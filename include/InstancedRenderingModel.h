#ifndef INSTANCEDRENDERINGMODEL_H
#define INSTANCEDRENDERINGMODEL_H

#include "Engine/VulkanVertexBuffer.h"
#include <map>

#include "Vertex.h"

struct InstanceSetData {
    VulkanVertexBuffer<InstanceData> data;
};


template<typename VertexType>
class InstancedRenderingModel {
    public:
        InstancedRenderingModel(std::shared_ptr<VulkanDevice> _device, std::vector<VertexType>& _verts) {
            model.setVertexData(_device, _verts);
        }

        void destroy(std::shared_ptr<VulkanDevice> _device, bool* shouldBeDestroyed) {
            model.destroy(_device, shouldBeDestroyed);

            for(std::pair<const std::string, InstanceSetData>& instanceData : instanceSets) {
                instanceData.second.data.destroy(_device, shouldBeDestroyed);
            }
        }

        void clearInstances(std::shared_ptr<VulkanDevice> _device, bool* shouldBeDestroyed) {
            for(std::pair<const std::string, InstanceSetData>& instanceData : instanceSets) {
                instanceData.second.data.destroy(_device, shouldBeDestroyed);
            }
            instanceSets.clear();
        }

        VulkanVertexBuffer<VertexType>& getModel() {
            return model;
        }

        std::map<std::string, InstanceSetData>& getInstanceSets() {
            return instanceSets;
        }

        void setModel(std::shared_ptr<VulkanDevice> _device, std::vector<VertexType>& mdl) {
            model.setVertexData(_device, mdl);
        }

        void addInstancesToModel(std::shared_ptr<VulkanDevice> _device, std::string instanceVectorID, std::vector<InstanceData>& instances) {
            if(instanceSets.count(instanceVectorID) > 0) {
                instanceSets[instanceVectorID].data.setVertexData(_device, instances);
                return;
            }

            VulkanVertexBuffer<InstanceData> instanceBuffer = VulkanVertexBuffer<InstanceData>();
            instanceBuffer.setVertexData(_device, instances);

            instanceSets[instanceVectorID].data = instanceBuffer;
        }

        void removeInstancesFromModel(std::shared_ptr<VulkanDevice> _device, std::string instanceVectorID, bool* shouldBeDestroyed) {
            instanceSets[instanceVectorID].data.destroy(_device, shouldBeDestroyed);
            instanceSets.erase(instanceVectorID);
        }

        VulkanVertexBuffer<InstanceData>& getInstanceSet(std::string set) {
            return instanceSets.at(set).data;
        }

        bool hasInstanceSet(std::string set) {
            return (instanceSets.count(set) != 0);
        }

    private:
        VulkanVertexBuffer<VertexType> model;
        std::map<std::string, InstanceSetData> instanceSets;
};

#endif