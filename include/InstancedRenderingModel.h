#ifndef INSTANCEDRENDERINGMODEL_H
#define INSTANCEDRENDERINGMODEL_H

#include "Engine/VulkanVertexBuffer.h"
#include <map>
#include "Vertex.h"

class InstancedRenderingModel {
    public:
        InstancedRenderingModel(std::shared_ptr<VulkanDevice> _device, std::vector<Vertex>& _verts) : device(_device) {
            model.setVertexData(device, _verts);
        }

        void destroy(bool* shouldBeDestroyed) {
            model.destroy(device, shouldBeDestroyed);

            for(std::pair<const std::string, VulkanVertexBuffer<InstanceData>>& instanceData : instanceSets) {
                instanceData.second.destroy(device, shouldBeDestroyed);
            }
        }

        void clearInstances(bool* shouldBeDestroyed) {
            for(std::pair<const std::string, VulkanVertexBuffer<InstanceData>>& instanceData : instanceSets) {
                instanceData.second.destroy(device, shouldBeDestroyed);
            }
            instanceSets.clear();
        }

        VulkanVertexBuffer<Vertex>& getModel() {
            return model;
        }

        std::map<std::string, VulkanVertexBuffer<InstanceData>>& getInstanceSets() {
            return instanceSets;
        }

        void setModel(std::vector<Vertex>& mdl) {
            model.setVertexData(device, mdl);
        }

        void addInstancesToModel(std::string instanceVectorID, std::vector<InstanceData>& instances) {
            if(instanceSets.count(instanceVectorID) > 0) {
                instanceSets[instanceVectorID].setVertexData(device, instances);
                return;
            }

            VulkanVertexBuffer<InstanceData> instanceBuffer = VulkanVertexBuffer<InstanceData>();
            instanceBuffer.setVertexData(device, instances);

            instanceSets[instanceVectorID] = instanceBuffer;
        }

        void removeInstancesFromModel(std::string instanceVectorID, bool* shouldBeDestroyed) {
            instanceSets[instanceVectorID].destroy(device, shouldBeDestroyed);
            instanceSets.erase(instanceVectorID);
        }

        VulkanVertexBuffer<InstanceData>& getInstanceSet(std::string set) {
            return instanceSets.at(set);
        }


    private:
        std::shared_ptr<VulkanDevice> device;
        VulkanVertexBuffer<Vertex> model;
        std::map<std::string, VulkanVertexBuffer<InstanceData>> instanceSets;
};

#endif