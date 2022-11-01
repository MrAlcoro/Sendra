#include "Application.h"
#include "ModuleLoader.h"
#include "cimport.h"
#include "cimport.h"
#include "scene.h"
#include "postprocess.h"
#include "cfileio.h"
#include "il.h"
#include "ilut.h"
#include "AABB.h"

#pragma comment (lib, "../Libraries/Assimp/libx86/assimp.lib")
#pragma comment (lib, "../Libraries/DevIL/libx86/DevIL.lib")
#pragma comment (lib, "../Libraries/DevIL/libx86/ILU.lib")
#pragma comment (lib, "../Libraries/DevIL/libx86/ILUT.lib")


ModuleLoader::ModuleLoader(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name.assign("loader");
}

ModuleLoader::~ModuleLoader() {}

// -----------------------------------------------------------------
bool ModuleLoader::Init()
{
	bool ret = true;

	ilInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return ret;
}

void ModuleLoader::SetTexture(const char* path)
{
	ILuint id;
	ilGenImages(1, &id);
	ilBindImage(id);
	ilLoadImage(path);
	last_texture_id = ilutGLBindTexImage();
}

// Pre update -------------------------------------------------------
update_status ModuleLoader::PreUpdate(float dt)
{
	return(UPDATE_CONTINUE);
}

// ------------------------------------------------------------------
update_status ModuleLoader::Update(float dt)
{
	return (UPDATE_CONTINUE);
}

// ------------------------------------------------------------------
void ModuleLoader::LoadFile(std::string file_path)
{
	const aiScene* scene = aiImportFile(file_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		aiNode* root = scene->mRootNode;

		for (int i = 0; i < root->mNumChildren; i++)
		{
			LoadMesh(scene, root->mChildren[i]);
		}

		aiReleaseImport(scene);

		LOG("Loading scene with path: %s", file_path.c_str());
	}
	else
	{
		LOG("Error loading scene %s", file_path.c_str());
	}
}

// ------------------------------------------------------------------
void ModuleLoader::LoadMesh(const aiScene* scene, aiNode* children_node)
{
	if (children_node->mNumMeshes >= 0)
	{
		for (int i = 0; i < children_node->mNumMeshes; ++i)
		{
			aiMesh* current_mesh = scene->mMeshes[children_node->mMeshes[i]];
			mesh_to_load = MeshData();

			mesh_to_load.num_vertex = current_mesh->mNumVertices;
			mesh_to_load.vertex = new uint[mesh_to_load.num_vertex * 3];

			memcpy(mesh_to_load.vertex, current_mesh->mVertices, sizeof(float) * mesh_to_load.num_vertex * 3);

			glGenBuffers(1, (GLuint*)&(mesh_to_load.id_vertex));
			glBindBuffer(GL_ARRAY_BUFFER, mesh_to_load.id_vertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh_to_load.num_vertex, mesh_to_load.vertex, GL_STATIC_DRAW);
			LOG("Mesh with %d vertices.", mesh_to_load.num_vertex);

			if (current_mesh->HasFaces())
			{
				mesh_to_load.num_index = current_mesh->mNumFaces * 3;
				mesh_to_load.index = new uint[mesh_to_load.num_index];

				for (uint j = 0; j < current_mesh->mNumFaces; ++j)
				{
					if (current_mesh->mFaces[j].mNumIndices != 3)
					{
						LOG("WARNING: geometry face with != 3 index!");
					}
					else
					{
						memcpy(&mesh_to_load.index[j * 3], current_mesh->mFaces[j].mIndices, 3 * sizeof(float));
					}
				}

				glGenBuffers(1, (GLuint*)&(mesh_to_load.id_index));
				glBindBuffer(GL_ARRAY_BUFFER, mesh_to_load.id_index);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_to_load.num_index, mesh_to_load.index, GL_STATIC_DRAW);
			}

			if (current_mesh->HasNormals())
			{
				mesh_to_load.num_normals = current_mesh->mNumVertices;
				mesh_to_load.normals = new float[mesh_to_load.num_normals * 3];
				memcpy(mesh_to_load.normals, current_mesh->mNormals, sizeof(float) * mesh_to_load.num_normals * 3);

				glGenBuffers(1, (GLuint*)&(mesh_to_load.id_normals));
				glBindBuffer(GL_ARRAY_BUFFER, mesh_to_load.id_normals);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh_to_load.num_normals, mesh_to_load.normals, GL_STATIC_DRAW);
			}

			if (current_mesh->HasTextureCoords(mesh_to_load.id_uvs))
			{
				mesh_to_load.num_uvs = current_mesh->mNumVertices;
				mesh_to_load.uvs = new float[mesh_to_load.num_uvs * 2];

				for (int i = 0; i < current_mesh->mNumVertices; ++i)
				{
					memcpy(&mesh_to_load.uvs[i * 2], &current_mesh->mTextureCoords[0][i].x, sizeof(float));
					memcpy(&mesh_to_load.uvs[(i * 2) + 1], &current_mesh->mTextureCoords[0][i].y, sizeof(float));
				}

				glGenBuffers(1, (GLuint*)&(mesh_to_load.id_uvs));
				glBindBuffer(GL_ARRAY_BUFFER, mesh_to_load.id_uvs);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh_to_load.num_uvs, mesh_to_load.uvs, GL_STATIC_DRAW);
			}

			aiMaterial* texture = scene->mMaterials[current_mesh->mMaterialIndex];

			if (texture == nullptr)
			{
				LOG("This model has no texture or its texture cannot be found.");
			}
			else
			{
				aiString texture_path;
				texture->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);

				if (texture_path.length > 0)
				{
					std::string base_path = "../../Models/";
					std::string final_path = texture_path.data;
					final_path.erase(0, final_path.find_last_of("\\") + 1);
					base_path += final_path;

					mesh_to_load.texture_id = GenerateTextureId(base_path.c_str());

					final_path.clear();
					base_path.clear();
				}

				LOG("Loading this mesh's texture.");
			}

			meshes.push_back(mesh_to_load);
			LOG("Loaded mesh with %i vertices.", mesh_to_load.num_vertex);
			LOG("Loaded mesh with %i indices.", mesh_to_load.num_index);
			LOG("Loaded mesh with %i triangles.", mesh_to_load.num_vertex / 3);
		}
	}
	else
	{
		LOG("This node has no meshes.");
	}

	for (int k = 0; k < children_node->mNumChildren; k++)
	{
		LoadMesh(scene, children_node->mChildren[k]);
	}
}

// Generate texture id ----------------------------------------------
uint ModuleLoader::GenerateTextureId(const char* texture_path)
{
	uint result;
	ILuint tmp_id;

	ilGenImages(1, &tmp_id);
	ilBindImage(tmp_id);
	ilLoadImage(texture_path);

	result = ilutGLBindTexImage();

	return result;
}

// ------------------------------------------------------------------
bool ModuleLoader::CleanUp()
{
	LOG("Cleaning fbx loader");

	aiDetachAllLogStreams();

	return true;
}

// Save & load ----------------------------------------------------------------------
bool ModuleLoader::Save()
{
	if (App->config != NULL)
	{
		if (json_object_has_value(App->modules_object, name.data()) == false)
		{
			json_object_set_null(App->modules_object, name.data());
			json_serialize_to_file_pretty(App->config, "config.json");
		}

		LOG("Saving module %s", name.data());
	}
	else
	{
		json_object_set_null(App->modules_object, name.data());

		LOG("Saving module %s", name.data());
	}


	return(true);
}

bool ModuleLoader::Load()
{
	bool ret = false;

	if (App->config != NULL)
	{
		if (json_object_has_value(App->modules_object, name.data()) != false)
		{
			LOG("Loading module %s", name.data());
			ret = true;
		}
		else
		{
			LOG("Could not find the node named %s inside the file config.json", name.data());
			ret = false;
		}
	}
	else
	{
		LOG("Document config.json not found.");
		ret = false;
	}

	return ret;
}

void ModuleLoader::Focus()
{
	math::AABB box(float3(0, 0, 0), float3(0, 0, 0));
	box.Enclose((float3*)App->loader->mesh_to_load.vertex, App->loader->mesh_to_load.num_vertex);

	App->camera->Reference.x = box.CenterPoint().x;
	App->camera->Reference.y = box.CenterPoint().y;
	App->camera->Reference.z = box.CenterPoint().z;

	App->camera->Position.x = box.maxPoint.x * 2;
	App->camera->Position.y = box.maxPoint.y * 2;
	App->camera->Position.z = box.maxPoint.z * 2;

	App->camera->LookAt(App->camera->Reference);
}