// OutlineEffectLibrary.cpp

#include "components/OutlineEffectLibrary.h"
#include "Components/PrimitiveComponent.h"

void UOutlineEffectLibrary::SetActorOutline(AActor* Target, bool bEnable)
{
	if (!Target) return;

	TArray<UStaticMeshComponent*> Meshes;
	Target->GetComponents<UStaticMeshComponent>(Meshes);

	for (auto Mesh : Meshes)
	{
		Mesh->SetRenderCustomDepth(bEnable);
		Mesh->SetCustomDepthStencilValue(252);  // ← 여기에 지정한 숫자는 마테리얼에서 식별용으로 사용됨
	}
}
