#pragma once 

#include "GLIncludes.h"
#include "Shader.cpp"
#include "Mesh.cpp"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "SOIL.h"

#include <vector>
#include "Mesh.cpp"

class Model {
private:
	std::vector<Mesh> meshes;
	std::vector<Mesh::Texture> textures_loaded;
	std::string dir;

	void loadModel(std::string path) {
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || !scene->mRootNode) {
			std::cout << "Assimp import error: " << import.GetErrorString() << std::endl;
			return;
		}

		this->dir = path.substr(0, path.find_last_of('/'));
		this->processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode* node, const aiScene* scene) {
		for (GLuint i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			this->meshes.push_back(this->processMesh(mesh, scene));
		}

		for (GLuint i = 0; i < node->mNumChildren; i++) {
			this->processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Mesh::Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Mesh::Texture> textures;

		vec3 vector;

		//process vertex positions, normals & texture coords
		for (GLuint i = 0; i < mesh->mNumVertices; i++) {
			Mesh::Vertex vertex;

			//pos
			vector.v[0] = mesh->mVertices[i].x;
			vector.v[1] = mesh->mVertices[i].y;
			vector.v[2] = mesh->mVertices[i].z;
			vertex.position = vector;

			//normals
			vector.v[0] = mesh->mNormals[i].x;
			vector.v[1] = mesh->mNormals[i].y;
			vector.v[2] = mesh->mNormals[i].z;
			vertex.normal = vector;

			//if has texture coords
			if (mesh->mTextureCoords[0]) {
				vec2 vec;
				vec.v[0] = mesh->mTextureCoords[0][i].x;
				vec.v[1] = mesh->mTextureCoords[0][i].y;
				vertex.texCoords = vec;
			}
			else {
				vertex.texCoords = vec2(0.0f, 0.0f);
			}

			vertices.push_back(vertex);
		}

		//process indices for each face
		for (GLuint i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (GLuint j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		//process if has material
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			//diffuse
			std::vector<Mesh::Texture> diffuse = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuse.begin(), diffuse.end());

			//specular
			std::vector<Mesh::Texture> specular = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), diffuse.begin(), diffuse.end());
		}

		return Mesh(vertices, indices, textures);
	}

	std::vector<Mesh::Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
		std::vector<Mesh::Texture> textures;
		for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);
			GLboolean skip = false;

			for (GLuint j = 0; j < textures_loaded.size(); j++) {
				if (textures_loaded[j].path == str) {
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip) {
				Mesh::Texture texture;
				texture.id = textureFromFile(str.C_Str(), this->dir);
				texture.type = typeName;
				texture.path = str;
				textures.push_back(texture);
				this->textures_loaded.push_back(texture);
			}
		}
		return textures;
	}

	GLint textureFromFile(const char* path, std::string directory) {
		//Generate texture ID and load texture data 
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		std::cout << filename << std::endl;
		
		GLuint textureID;
		glGenTextures(1, &textureID);
		int width, height;
		unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		
		// Assign texture to ID
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		SOIL_free_image_data(image);
		return textureID;
	}

public:
	Model() {}
	Model(GLchar* path) {
		this->loadModel(path);
	}

	~Model() {}

	void draw(Shader shader) {
		for (GLuint i = 0; i < this->meshes.size(); i++) {
			this->meshes[i].draw(shader);
		}
	}
};