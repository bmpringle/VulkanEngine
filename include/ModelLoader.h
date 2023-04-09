#ifndef MODELLOADER_H
#define MODELLOADER_H

#include "Vertex.h"

#include <vector>

//returns the vertices and the texture file path
std::pair<std::vector<Vertex>, std::string> import_model(std::string relative_path_to_resources, std::string model_file);

#endif