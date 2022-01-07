#ifndef INSTANCEDRENDERINGMODEL_H
#define INSTANCEDRENDERINGMODEL_H

#include "Engine/VulkanVertexBuffer.h"
#include <map>
#include "Vertex.h"

struct InstanceSetData {
    VulkanVertexBuffer<InstanceData> data;
    bool isWireframe = false;
};


class InstancedRenderingModel {
    public:
        InstancedRenderingModel(std::shared_ptr<VulkanDevice> _device, std::vector<Vertex>& _verts) : device(_device) {
            model.setVertexData(device, _verts);
        }

        void destroy(bool* shouldBeDestroyed) {
            model.destroy(device, shouldBeDestroyed);

            for(std::pair<const std::string, InstanceSetData>& instanceData : instanceSets) {
                instanceData.second.data.destroy(device, shouldBeDestroyed);
            }
        }

        void clearInstances(bool* shouldBeDestroyed) {
            for(std::pair<const std::string, InstanceSetData>& instanceData : instanceSets) {
                instanceData.second.data.destroy(device, shouldBeDestroyed);
            }
            instanceSets.clear();
        }

        VulkanVertexBuffer<Vertex>& getModel() {
            return model;
        }

        std::map<std::string, InstanceSetData>& getInstanceSets() {
            return instanceSets;
        }

        void setModel(std::vector<Vertex>& mdl) {
            model.setVertexData(device, mdl);
        }

        void addInstancesToModel(std::string instanceVectorID, std::vector<InstanceData>& instances, bool wireframeInstances = false) {
            if(instanceSets.count(instanceVectorID) > 0) {
                instanceSets[instanceVectorID].data.setVertexData(device, instances);
                return;
            }

            VulkanVertexBuffer<InstanceData> instanceBuffer = VulkanVertexBuffer<InstanceData>();
            instanceBuffer.setVertexData(device, instances);

            instanceSets[instanceVectorID].data = instanceBuffer;
            instanceSets[instanceVectorID].isWireframe = wireframeInstances;
        }

        void removeInstancesFromModel(std::string instanceVectorID, bool* shouldBeDestroyed) {
            instanceSets[instanceVectorID].data.destroy(device, shouldBeDestroyed);
            instanceSets.erase(instanceVectorID);
        }

        VulkanVertexBuffer<InstanceData>& getInstanceSet(std::string set) {
            return instanceSets.at(set).data;
        }

        void setIsRenderedAsWireframe(std::string instanceVectorID, bool wireframe) {
            instanceSets[instanceVectorID].isWireframe = wireframe;
        }

    private:
        std::shared_ptr<VulkanDevice> device;
        VulkanVertexBuffer<Vertex> model;
        std::map<std::string, InstanceSetData> instanceSets;
};

#endif