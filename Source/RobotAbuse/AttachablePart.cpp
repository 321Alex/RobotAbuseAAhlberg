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

//TODO Would like to add a separate component to handle highlight and make it more flexible
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
    // When clicked, pick ourselves up
    if (CurrentState == EPartState::DETACHED)
    {
        PickUp();
    }
    else if (CurrentState == EPartState::ATTACHED)
    {
        DetachFromPoint();
        PickUp();
    }
}

void AAttachablePart::OnDropped_Implementation()
{
    Drop();
}

bool AAttachablePart::TryAttachTo_Implementation(UAttachmentPoint* Point)
{
    // Validate inputs
    if (!Point)
    {
        UE_LOG(LogTemp, Warning, TEXT("TryAttachTo called with null point"));
        return false;
    }
    
    // Check if this part can attach to this point (type compatibility)
    if (!Point->CanAcceptPart(this))
    {
        UE_LOG(LogTemp, Warning, TEXT("%s cannot attach to %s - wrong type"), 
               *GetName(), *Point->GetName());
        return false;
    }
    
    // Detach from current point if attached elsewhere
    if (CurrentAttachmentPoint)
    {
        DetachFromPoint();
    }
    
    // Perform attachment
    Point->AttachPart(this);      // Tell point it has a part
    AttachToPoint(Point);          // Attach ourselves to the point
    
    UE_LOG(LogTemp, Log, TEXT("Successfully attached %s to %s"), 
           *GetName(), *Point->GetName());
    
    return true;
}

void AAttachablePart::PickUp()
{
    CurrentState = EPartState::HELD;
    SetEmissive(HighlightEmissive);
}

void AAttachablePart::Drop()
{
    CurrentState = EPartState::DETACHED;
    SetEmissive(NormalEmissive);
}

void AAttachablePart::AttachToPoint(UAttachmentPoint* Point)
{
    CurrentAttachmentPoint = Point;
    CurrentState = EPartState::ATTACHED;
    
    // Snap to attachment point location
    AttachToComponent(Point, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    SetActorLocation(Point->GetComponentLocation());
    
    // Turn off highlight
    SetEmissive(NormalEmissive);
}

void AAttachablePart::DetachFromPoint()
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    
    if (CurrentAttachmentPoint)
    {
        CurrentAttachmentPoint->DetachPart();
        CurrentAttachmentPoint = nullptr;
    }
    
    CurrentState = EPartState::DETACHED;
}


void AAttachablePart::UpdateDragPosition_Implementation(const FVector& WorldPosition)
{
    //Added offset to handle offset on mesh but did not get a chance to configure well
    if (CurrentState == EPartState::HELD)
    {
        SetActorLocation(WorldPosition + HeldOffset);
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

