#include "Model.h"



Model::Model(VkDeviceSize index, const std::string modelPath, Material* mat, uint32_t instanceCount)
{
	instanceData.resize(instanceCount);
	this->material = mat;
	this->instanceCount = instanceCount;
	loadModel(modelPath);
	this->index = index;
	vertexOffset = index;
	vertexSizeBytes = sizeof(vertices[0]) * vertices.size();
	indexSizeBytes = sizeof(indices[0]) * indices.size();
	instanceDataSizeBytes = sizeof(instanceData[0]) * instanceData.size();
	indexOffset = vertexOffset + vertexSizeBytes;
	modelSizeBytes = vertexSizeBytes + indexSizeBytes;
	instanceDataOffset = modelSizeBytes;
	modelSizeBytes += instanceDataSizeBytes;
	range = index + modelSizeBytes;
	
	std::cout << "Range: " << range << ", Model Size Bytes: " << modelSizeBytes << std::endl;
}


Model::~Model()
{
}



void Model::loadModel(const std::string modelPath)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, modelPath.c_str())) {
		throw std::runtime_error(err);
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };
			// TODO: combine all mesh vertices and indices into one to prevent multiple vkcmdbindvertex/indexbuffer calls in one frame
			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}

}
