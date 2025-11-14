#include "AttachablePart.h"
#include "AttachmentPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

AAttachablePart::AAttachablePart()
{
    PrimaryActorTick.bCanEverTick = true;
    //Creates the root mesh for our robot part
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = MeshComponent;

    MeshComponent->SetSimulatePhysics(false);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    CurrentState = EPartState::DETACHED;
}

void AAttachablePart::BeginPlay()
{
    Super::BeginPlay();
    
    SetupMaterials();
    
    // Set initial emissive
    SetEmissive(NormalEmissive);
}

//TODO Would like to add a seperate component to handle highlight and make it more flexible
void AAttachablePart::SetupMaterials()
{
    DynamicMaterials.Empty();
    
    // Get all mesh components in this actor (including children)
    TArray<UStaticMeshComponent*> MeshComponents;
    GetComponents<UStaticMeshComponent>(MeshComponents);
    
    // Create dynamic material instances for each mesh
    for (UStaticMeshComponent* Mesh : MeshComponents)
    {
        if (Mesh)
        {
            // For each material slot on this mesh
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

void AAttachablePart::OnHoverBegin_Implementation()
{
    if (CurrentState != EPartState::HELD)
    {
        SetEmissive(HighlightEmissive);
    }
}

void AAttachablePart::OnHoverEnd_Implementation()
{
    if (CurrentState != EPartState::HELD)
    {
        SetEmissive(NormalEmissive);
    }
}

void AAttachablePart::OnClicked_Implementation()
{
    PickUp();
}

void AAttachablePart::PickUp()
{
    //When this object is picked up it checks if needs to be detached from point
    DetachFromCurrentPoint();
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    
    CurrentState = EPartState::HELD;
    SetEmissive(HighlightEmissive);
    
    UE_LOG(LogTemp, Log, TEXT("Picked up part: %s"), *GetName());
}


void AAttachablePart::Drop()
{
    CurrentState = EPartState::DETACHED;
    SetEmissive(NormalEmissive);
}

void AAttachablePart::AttachToPoint(UAttachmentPoint* Point)
{
    if (!Point) return;
    
    DetachFromCurrentPoint();
    CurrentAttachmentPoint = Point;
    
    AttachToComponent(Point, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    
    CurrentState = EPartState::ATTACHED;
    SetEmissive(NormalEmissive);
    
    UE_LOG(LogTemp, Log, TEXT("Part %s attached to point %s"), *GetName(), *Point->GetName());
}



void AAttachablePart::UpdateHeldPosition(FVector WorldPosition)
{
    //Added offset to handle offset on mesh but did not get a chance to configure well
    if (CurrentState == EPartState::HELD)
    {
        SetActorLocation(WorldPosition + HeldOffset);
    }
}

void AAttachablePart::DetachFromCurrentPoint()
{
    if (CurrentAttachmentPoint)
    {
        // Tell the attachment point we're detaching
        CurrentAttachmentPoint->DetachPart();
        CurrentAttachmentPoint = nullptr;
    }
}

void AAttachablePart::SetEmissive(float Value)
{
    // Set emissive on all materials
    for (UMaterialInstanceDynamic* Mat : DynamicMaterials)
    {
        if (Mat)
        {
            Mat->SetScalarParameterValue(EmissiveParameterName, Value);
        }
    }
}

