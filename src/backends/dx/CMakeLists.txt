if (WIN32)
    
    message(STATUS "Build with DirectX")
    
    set(DX_BACKEND_SOURCES
        CJsonObject/cJSON.h
        CJsonObject/JsonCompiler.h
        CJsonObject/SerializedObject.h
        CJsonObject/SerializeStruct.h
        Common/BitArray.h
        Common/BitVector.h
        Common/Camera.h
        Common/Common.h
        Common/d3dx12.h
        Common/DLL.h
        Common/DXMath/Common.h
        Common/DXMath/DXMath.h
        Common/DXMath/Matrix3.h
        Common/DXMath/Matrix4.h
        Common/DXMath/Quaternion.h
        Common/DXMath/Scalar.h
        Common/DXMath/Vector.h
        Common/DynamicDLL.h
        Common/GFXUtil.h
        Common/Hash.h
        Common/HashMap.h
        Common/IObjectReference.h
        Common/LinkedList.h
        Common/LockFreeArrayQueue.h
        Common/Log.h
        Common/MathHelper.h
        Common/MCollection.h
        Common/Memory.h
        Common/MetaLib.h
        Common/Pool.h
        Common/PoolAllocator.h
        Common/RandomVector.h
        Common/Runnable.h
        Common/string_view.h
        Common/Tuple.h
        Common/TypeWiper.h
        Common/vector.h
        Common/VObject.h
        Common/vstring.h
        File/CopyDir.h
        File/FilePacker.h
        File/FileUtility.h
        File/MGUID.h
        File/Path.h
        JobSystem/JobBucket.h
        JobSystem/JobHandle.h
        JobSystem/JobInclude.h
        JobSystem/JobNode.h
        JobSystem/JobSystem.h
        LogicComponent/Component.h
        LogicComponent/RayRenderer.h
        LogicComponent/RendererBase.h
        LogicComponent/Transform.h
        PipelineComponent/CameraData/CameraTransformData.h
        PipelineComponent/CameraData/LightCBuffer.h
        PipelineComponent/CameraData/LightMacro.h
        PipelineComponent/CommandAllocator.h
        PipelineComponent/CommandBuffer.h
        PipelineComponent/CommandListType.h
        PipelineComponent/IPipelineResource.h
        PipelineComponent/RayTracing/RayTracingManager.h
        PipelineComponent/ThreadCommand.h
        RenderComponent/BackBuffer.h
        RenderComponent/CBufferAllocator.h
        RenderComponent/CBufferPool.h
        RenderComponent/ComputeShader.h
        RenderComponent/DescriptorHeap.h
        RenderComponent/DescriptorHeapRoot.h
        RenderComponent/GPUResourceBase.h
        RenderComponent/IBackBuffer.h
        RenderComponent/IBuffer.h
        RenderComponent/IMesh.h
        RenderComponent/IShader.h
        RenderComponent/Mesh.h
        RenderComponent/PSOContainer.h
        RenderComponent/RayRendererData.h
        RenderComponent/RayShader.h
        RenderComponent/ReadbackBuffer.h
        RenderComponent/RenderCommand.h
        RenderComponent/RenderComponentInclude.h
        RenderComponent/RenderTexture.h
        RenderComponent/Shader.h
        RenderComponent/Skybox.h
        RenderComponent/StructuredBuffer.h
        RenderComponent/Texture.h
        RenderComponent/TextureBase.h
        RenderComponent/TextureHeap.h
        RenderComponent/UploadBuffer.h
        RenderComponent/Utility/CommandSignature.h
        RenderComponent/Utility/CullingMask.h
        RenderComponent/Utility/D3D12MemoryAllocator/D3D12MemAlloc.h
        RenderComponent/Utility/DefaultBufferAllocator.h
        RenderComponent/Utility/DefaultTextureAllocator.h
        RenderComponent/Utility/IBufferAllocator.h
        RenderComponent/Utility/ITextureAllocator.h
        RenderComponent/Utility/RendererCull.h
        RenderComponent/Utility/ShaderIO.h
        RenderComponent/Voxel/VirtualVoxelMap.h
        RHI/IBuffer.h
        RHI/ICommandBuffer.h
        RHI/IComputeShader.h
        RHI/IGFXDevice.h
        RHI/IGpuResource.h
        RHI/IMesh.h
        RHI/IRayTracingShader.h
        RHI/IShader.h
        RHI/ITexture.h
        RHI/Transform.h
        ShaderCompile/ASTEncoder.h
        ShaderCompile/HLSLCompiler.h
        ShaderCompile/LuisaASTTranslator.h
        ShaderCompile/ShaderUniforms.h
        Singleton/ColorUtility.h
        Singleton/Graphics.h
        Singleton/MathLib.h
        Singleton/MeshLayout.h
        Singleton/ShaderID.h
        Singleton/ShaderLoader.h
        Struct/CameraRenderPath.h
        Struct/CopyBufferCommand.h
        Struct/CullResult.h
        Struct/DrawCommand.h
        Struct/ObjectCBufferParam.h
        Struct/RenderPackage.h
        Struct/RenderTarget.h
        Struct/ShaderVariableType.h
        Utility/Actor.h
        Utility/BinaryReader.h
        Utility/BuddyAllocator.h
        Utility/ElementAllocator.h
        Utility/IEnumerator.h
        Utility/MD5.h
        Utility/PassConstants.h
        Utility/QuickSort.h
        Utility/Random.h
        Utility/StringUtility.h
        Utility/TaskThread.h
        Utility/ThreadResetEvent.h
        WorldManagement/World.h
        CJsonObject/cJSON.cpp
        CJsonObject/CJsonObject.cpp
        CJsonObject/JsonCompiler.cpp
        CJsonObject/SerializedObject.cpp
        Common/Camera.cpp
        Common/Common.cpp
        Common/GFXUtil.cpp
        Common/Log.cpp
        Common/MathHelper.cpp
        Common/vengine_library.cpp
        Common/VObject.cpp
        File/CopyDir.cpp
        File/FilePacker.cpp
        File/FileUtility.cpp
        File/MGUID.cpp
        File/Path.cpp
        JobSystem/JobBucket.cpp
        JobSystem/JobNode.cpp
        JobSystem/JobSystem.cpp
        LogicComponent/Component.cpp
        LogicComponent/RayRenderer.cpp
        LogicComponent/RendererBase.cpp
        LogicComponent/Transform.cpp
        PipelineComponent/CommandAllocator.cpp
        PipelineComponent/CommandBuffer.cpp
        PipelineComponent/RayTracing/RayTracingManager.cpp
        PipelineComponent/ThreadCommand.cpp
        RenderComponent/BackBuffer.cpp
        RenderComponent/CBufferAllocator.cpp
        RenderComponent/CBufferPool.cpp
        RenderComponent/ComputeShader.cpp
        RenderComponent/DescriptorHeap.cpp
        RenderComponent/DescriptorHeapRoot.cpp
        RenderComponent/GPUResourceBase.cpp
        RenderComponent/Mesh.cpp
        RenderComponent/PSOContainer.cpp
        RenderComponent/RayShader.cpp
        RenderComponent/ReadbackBuffer.cpp
        RenderComponent/RenderCommand.cpp
        RenderComponent/RenderTexture.cpp
        RenderComponent/Shader.cpp
        RenderComponent/Skybox.cpp
        RenderComponent/StructuredBuffer.cpp
        RenderComponent/Texture.cpp
        RenderComponent/TextureBase.cpp
        RenderComponent/TextureHeap.cpp
        RenderComponent/UploadBuffer.cpp
        RenderComponent/Utility/CommandSignature.cpp
        RenderComponent/Utility/D3D12MemoryAllocator/D3D12MemAlloc.cpp
        RenderComponent/Utility/DefaultBufferAllocator.cpp
        RenderComponent/Utility/DefaultTextureAllocator.cpp
        RenderComponent/Utility/ShaderIO.cpp
        RenderComponent/Voxel/VirtualVoxelMap.cpp
        ShaderCompile/ASTEncoder.cpp
        ShaderCompile/HLSLCompiler.cpp
        ShaderCompile/LuisaASTTranslator.cpp
        Singleton/ColorUtility.cpp
        Singleton/Graphics.cpp
        Singleton/MathLib.cpp
        Singleton/MeshLayout.cpp
        Singleton/ShaderID.cpp
        Singleton/ShaderLoader.cpp
        TestMain.cpp
        Utility/Actor.cpp
        Utility/BinaryReader.cpp
        Utility/BuddyAllocator.cpp
        Utility/ElementAllocator.cpp
        Utility/IEnumerator.cpp
        Utility/MD5.cpp
        Utility/Random.cpp
        Utility/StringUtility.cpp
        Utility/TaskThread.cpp
        Utility/ThreadResetEvent.cpp
        WorldManagement/World.cpp)
    
    luisa_compute_add_backend(dx SOURCES ${DX_BACKEND_SOURCES})
    target_include_directories(luisa-compute-backend-dx PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    
    if (MSVC)
        target_compile_options(luisa-compute-backend-dx PRIVATE
                               /JMC
                               /permissive-
                               /MP
                               /GS /TP /Qpar /GL /W0 /Gy
                               /Zi /Gm-
                               /sdl- /GT
                               $<$<NOT:$<CXX_COMPILER_ID:Clang>>:/QIntel-jcc-erratum>
                               /WX- /Zc:forScope
                               /Zc:inline /fp:precise
                               /arch:AVX2 /Gr /Oy /Oi
                               /std:c++latest /FC
                               /nologo
                               /Ot)
        
        target_link_options(luisa-compute-backend-dx PRIVATE
                            /MANIFEST /LTCG:incremental /NXCOMPAT
                            /OPT:REF /INCREMENTAL:NO
                            /NOLOGO
                            /TLBID:1
                            /OPT:ICF /ERRORREPORT:PROMPT)
    endif ()
    
    target_link_libraries(luisa-compute-backend-dx PRIVATE luisa-compute-ast)
    set_target_properties(luisa-compute-backend-dx PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS OFF)

endif ()
