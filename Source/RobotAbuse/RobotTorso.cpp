#include "RobotTorso.h"

#include "AttachablePart.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ARobotTorso::ARobotTorso()
{
	PrimaryActorTick.bCanEverTick = false;

	RootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMesh"));
	RootComponent = RootMesh;
}

void ARobotTorso::BeginPlay()
{
	Super::BeginPlay();
	SetupMaterials();
	SetEmissive(NormalEmissive);
}

void ARobotTorso::SetupMaterials()
{
	DynamicMaterials.Empty();
    
	// Get all mesh components
	TArray<UStaticMeshComponent*> MeshComponents;
	GetComponents<UStaticMeshComponent>(MeshComponents);
    
	for (UStaticMeshComponent* Mesh : MeshComponents)
	{
		if (Mesh)
		{
			for (int32 i = 0; i < Mesh->GetNumMaterials(); i++)
			{
				UMaterialInterface* BaseMat = Mesh->GetMaterial(i);
				if (BaseMat)
				{
					UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(BaseMat, this);
					Mesh->SetMaterial(i, DynMat);
					DynamicMaterials.Add(DynMat);
				}
			}
		}
	}
}



void ARobotTorso::SetEmissive(float Value)
{
	for (UMaterialInstanceDynamic* Mat : DynamicMaterials)
	{
		if (Mat)
		{
			Mat->SetScalarParameterValue(EmissiveParameterName, Value);
		}
	}
}

void ARobotTorso::SetEmissiveIncludingAttachedParts(float Value)
{
	// Set emissive on torso
	SetEmissive(Value);
    
	// Get all attached actors (the arms)
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
    
	for (AActor* AttachedActor : AttachedActors)
	{
		// Check if it's an attachable part
		if (AAttachablePart* Part = Cast<AAttachablePart>(AttachedActor))
		{
			// Tell the part to highlight itself
			if (Value > NormalEmissive)
			{
				Part->OnHoverBegin_Implementation();
			}
			else
			{
				Part->OnHoverEnd_Implementation();
			}
		}
	}
}

void ARobotTorso::OnHoverBegin_Implementation()
{
	SetEmissiveIncludingAttachedParts(HighlightEmissive);
}

void ARobotTorso::OnHoverEnd_Implementation()
{
	SetEmissiveIncludingAttachedParts(NormalEmissive);
}

void ARobotTorso::OnClicked_Implementation()
{

}

