#pragma once

#include "WebGPUBuffer.h"
#include "WebGPUTexture.h"

namespace Figment
{
    class WebGPUCommand
    {
    public:
        WebGPUCommand() = delete;
        ~WebGPUCommand() = delete;

        template<typename T>
        static void CopyBufferToBuffer(WGPUDevice device, WebGPUBuffer<T> &from, WebGPUBuffer<T> &to, uint64_t size)
        {
            auto commandEncoder = CreateCommandEncoder(device, "CopyCommandEncoder");
            wgpuCommandEncoderCopyBufferToBuffer(commandEncoder, from.GetBuffer(), 0, to.GetBuffer(), 0, size);
            auto commandBuffer = CommandEncoderFinish(commandEncoder, "CopyCommandBuffer");
            SubmitCommandBuffer(device, commandBuffer);
            DestroyCommandEncoder(commandEncoder);
            DestroyCommandBuffer(commandBuffer);
        }

        template<typename T>
        static void CopyImageToBuffer(WGPUDevice device, WebGPUTexture &texture, WebGPUBuffer<T> &buffer,
                uint32_t width,
                uint32_t height)
        {
            WGPUImageCopyTexture imageCopyTexture = {
                    .nextInChain = nullptr,
                    .texture = texture.GetTexture(),
                    .mipLevel = 0,
                    .origin = { 0, 0, 0 },
                    .aspect = WGPUTextureAspect_All
            };

            WGPUImageCopyBuffer imageCopyBuffer = {
                    .nextInChain = nullptr,
                    .layout = {
                            .nextInChain = nullptr,
                            .offset = 0,
                            .bytesPerRow = width * sizeof(int32_t),
                            .rowsPerImage = height,
                    },
                    .buffer = buffer.GetBuffer(),
            };

            WGPUExtent3D copySize = {};
            copySize.width = texture.GetWidth();
            copySize.height = texture.GetHeight();
            copySize.depthOrArrayLayers = 1;

            auto commandEncoder = CreateCommandEncoder(device, "CopyCommandEncoder");
            wgpuCommandEncoderCopyTextureToBuffer(commandEncoder, &imageCopyTexture, &imageCopyBuffer, &copySize);
            auto commandBuffer = CommandEncoderFinish(commandEncoder, "CopyCommandBuffer");
            SubmitCommandBuffer(device, commandBuffer);
            DestroyCommandEncoder(commandEncoder);
            DestroyCommandBuffer(commandBuffer);
        }

        template<typename T>
        static void ClearBuffer(WGPUDevice device, WebGPUBuffer<T> &buffer)
        {
            auto commandEncoder = CreateCommandEncoder(device, "ClearBufferCommandEncoder");
            wgpuCommandEncoderClearBuffer(commandEncoder, buffer.GetBuffer(), 0, buffer.GetSize());
            auto commandBuffer = CommandEncoderFinish(commandEncoder, "ClearBufferCommandBuffer");
            SubmitCommandBuffer(device, commandBuffer);
            DestroyCommandEncoder(commandEncoder);
            DestroyCommandBuffer(commandBuffer);
        }

        static void SubmitCommandBuffer(WGPUDevice device, WGPUCommandBuffer commandBuffer)
        {
            WGPUQueue queue = wgpuDeviceGetQueue(device);
            wgpuQueueSubmit(queue, 1, &commandBuffer);
        }

        static WGPUCommandEncoder CreateCommandEncoder(WGPUDevice device, const char *label = "CommandEncoder")
        {
            WGPUCommandEncoderDescriptor commandEncoderDesc = {};
            commandEncoderDesc.nextInChain = nullptr;
            commandEncoderDesc.label = label;
            return wgpuDeviceCreateCommandEncoder(device, &commandEncoderDesc);
        }

        static WGPUCommandBuffer CommandEncoderFinish(WGPUCommandEncoder commandEncoder,
                const char *label = "CommandBuffer")
        {
            WGPUCommandBufferDescriptor commandBufferDesc = {};
            commandBufferDesc.nextInChain = nullptr;
            commandBufferDesc.label = label;
            return wgpuCommandEncoderFinish(commandEncoder, &commandBufferDesc);
        }

        static void DestroyCommandEncoder(WGPUCommandEncoder commandEncoder)
        {
            wgpuCommandEncoderRelease(commandEncoder);
        }

        static void DestroyCommandBuffer(WGPUCommandBuffer commandBuffer)
        {
            wgpuCommandBufferRelease(commandBuffer);
        }

        template<typename T>
        static void DrawIndexed(WGPURenderPassEncoder renderPass, WGPURenderPipeline pipeline, WGPUBindGroup bindGroup,
                WebGPUIndexBuffer<uint32_t> &indexBuffer, WebGPUVertexBuffer<T> &vertexBuffer, uint32_t indexCount)
        {
            wgpuRenderPassEncoderSetIndexBuffer(renderPass, indexBuffer.GetBuffer(), WGPUIndexFormat_Uint32, 0,
                    indexBuffer.GetSize());
            wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, vertexBuffer.GetBuffer(), 0, vertexBuffer.GetSize());
            wgpuRenderPassEncoderSetPipeline(renderPass, pipeline);
            wgpuRenderPassEncoderSetBindGroup(renderPass, 0, bindGroup, 0, nullptr);
            wgpuRenderPassEncoderDrawIndexed(renderPass, indexCount, 1, 0, 0, 0);
        }

        template<typename T>
        static void DrawVertices(WGPURenderPassEncoder renderPass, WGPURenderPipeline pipeline, WGPUBindGroup bindGroup,
                WebGPUVertexBuffer<T> &vertexBuffer, uint32_t vertexCount)
        {
            wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, vertexBuffer.GetBuffer(), 0, vertexBuffer.GetSize());
            wgpuRenderPassEncoderSetPipeline(renderPass, pipeline);
            wgpuRenderPassEncoderSetBindGroup(renderPass, 0, bindGroup, 0, nullptr);
            wgpuRenderPassEncoderDraw(renderPass, vertexCount, 1, 0, 0);
        }
    };
}
