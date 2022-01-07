python compileShaderPair.py -v shaders/src/2dshader.vert -f shaders/src/2dshader.frag -vo shaders/output/2dvert.spv -fo shaders/output/2dfrag.spv
python compileShaderPair.py -v shaders/src/3dshader.vert -f shaders/src/3dshader.frag -vo shaders/output/3dvert.spv -fo shaders/output/3dfrag.spv
python compileShaderPair.py -v shaders/src/3dshader_instanced.vert -f shaders/src/3dshader_instanced.frag -vo shaders/output/3dvert_instanced.spv -fo shaders/output/3dfrag_instanced.spv
python compileShaderPair.py -v shaders/src/3dshader_instanced_texArray.vert -f shaders/src/3dshader_instanced_texArray.frag -vo shaders/output/3dvert_instanced_texArray.spv -fo shaders/output/3dfrag_instanced_texArray.spv
python compileShaderPair.py -v shaders/src/3dshader_instanced_wireframe.vert -f shaders/src/3dshader_instanced_wireframe.frag -vo shaders/output/3dvert_instanced_wireframe.spv -fo shaders/output/3dfrag_instanced_wireframe.spv
python compileShaderPair.py -v shaders/src/overlay.vert -f shaders/src/overlay.frag -vo shaders/output/vert_overlay.spv -fo shaders/output/frag_overlay.spv