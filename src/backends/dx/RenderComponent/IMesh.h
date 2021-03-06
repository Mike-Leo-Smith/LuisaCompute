#pragma once
#include <RenderComponent/GPUResourceBase.h>
#include <Common/IObjectReference.h>
#include <RenderComponent/IGPUResourceState.h>
namespace luisa::compute {
class IMesh : public IObjectReference {
public:
	virtual uint GetIndexCount() const = 0;
	virtual GFXFormat GetIndexFormat() const = 0;
	virtual uint GetLayoutIndex() const = 0;
	virtual uint GetVertexCount() const = 0;
	virtual uint GetVBOSRVDescIndex(GFXDevice* device) const = 0;
	virtual uint GetIBOSRVDescIndex(GFXDevice* device) const = 0;
	virtual GFXVertexBufferView const* VertexBufferViews() const = 0;
	virtual uint VertexBufferViewCount() const = 0;
	virtual GFXIndexBufferView const* IndexBufferView() const = 0;

};
}// namespace luisa::compute