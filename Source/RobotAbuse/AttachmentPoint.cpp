#include "AttachmentPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ChildActorComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "AttachablePart.h"

UAttachmentPoint::UAttachmentPoint()
{
    PrimaryComponentTick.bCanEverTick = false;
    bVisualizeComponent = true;
}

void UAttachmentPoint::OnRegister()
{
	//OnRegister allows us to see if the connection is found outside of runtime
    Super::OnRegister();
    FindAttachmentVisual();
}

void UAttachmentPoint::BeginPlay()
{
    Super::BeginPlay();

    if (!AttachmentVisual)
    {
       FindAttachmentVisual();
    }

    RegisterInitialPart();
    SetupVisual();
}

void UAttachmentPoint::FindAttachmentVisual()
{
	// NOTE: Currently searches for visual in child comps. 
	// A more robust approach would use a named component or UPROPERTY reference.
    TArray<USceneComponent*> Children;
    GetChildrenComponents(false, Children);

    for (USceneComponent* Child : Children)
    {
       if (UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Child))
       {
          AttachmentVisual = MeshComp;
          UE_LOG(LogTemp, Log, TEXT("Found attachment visual: %s"), *MeshComp->GetName());
          break;
       }
    }

    if (!AttachmentVisual)
    {
       UE_LOG(LogTemp, Warning, TEXT("AttachmentPoint %s has no StaticMeshComponent child for visualization"),
              *GetName());
    }
}

void UAttachmentPoint::RegisterInitialPart()
{
	// NOTE: Currently searches for arm in child comps. 
	// A more robust approach would use a named component or UPROPERTY reference.
    TArray<USceneComponent*> Children;
    GetChildrenComponents(false, Children);
    
    for (USceneComponent* Child : Children)
    {
       if (UChildActorComponent* ChildActorComp = Cast<UChildActorComponent>(Child))
       {
          AActor* ChildActor = ChildActorComp->GetChildActor();
          if (ChildActor)
          {
             AAttachablePart* Part = Cast<AAttachablePart>(ChildActor);
             if (Part)
             {
                // Just set the part reference - that's all we need
                AttachedPart = Part;
                
                // Tell the part it's attached
                Part->AttachToPoint(this);
                    
                UE_LOG(LogTemp, Log, TEXT("Registered initial arm %s at attachment point %s"), 
                   *Part->GetName(), *GetName());
                    
                break;
             }
          }
       }
    }
}

void UAttachmentPoint::SetupVisual()
{
    if (AttachmentVisual)
    {
       // Hide visual if we have an attached part, show if empty
       bool bShouldShow = IsAvailable();
       AttachmentVisual->SetVisibility(bShouldShow);
       
       UE_LOG(LogTemp, Log, TEXT("AttachmentPoint %s visual visibility: %s (IsAvailable: %s)"), 
          *GetName(), 
          bShouldShow ? TEXT("TRUE") : TEXT("FALSE"),
          IsAvailable() ? TEXT("TRUE") : TEXT("FALSE"));
    }
}

void UAttachmentPoint::AttachPart(AAttachablePart* Part)
{
	if (!CanAcceptPart(Part))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot attach %s to %s - incompatible arm type"), 
			   *Part->GetName(), *GetName());
		return;
	}

	AttachedPart = Part;
	ShowAttachmentVisual(false);
    
	UE_LOG(LogTemp, Log, TEXT("Part %s attached to %s"), *Part->GetName(), *GetName());
}

void UAttachmentPoint::DetachPart()
{
    if (AttachedPart)
    {
       UE_LOG(LogTemp, Log, TEXT("Part %s detached from %s"), *AttachedPart->GetName(), *GetName());

       AttachedPart = nullptr;

       ShowAttachmentVisual(true);
       SetHighlighted(false);
    }
}

bool UAttachmentPoint::CanAcceptPart(AAttachablePart* Part) const
{
	if (!Part || !IsAvailable())
	{
		return false;
	}
    
	// Universal parts fit anywhere, universal sockets accept anything
	if (AcceptedArmType == EArmType::Universal || Part->ArmType == EArmType::Universal)
	{
		return true;
	}
    
	// Otherwise, types must match
	return AcceptedArmType == Part->ArmType;
}

void UAttachmentPoint::ShowAttachmentVisual(bool bShow)
{
    if (AttachmentVisual)
    {
       AttachmentVisual->SetVisibility(bShow);
       UE_LOG(LogTemp, Log, TEXT("AttachmentPoint %s visual set to: %s"), 
          *GetName(), 
          bShow ? TEXT("VISIBLE") : TEXT("HIDDEN"));
    }
}

void UAttachmentPoint::SetHighlighted(bool bHighlight)
{
    if (VisualMaterial)
    {
       float TargetIntensity = bHighlight ? HighlightIntensity : NormalIntensity;
       VisualMaterial->SetScalarParameterValue(EmissiveParameterName, TargetIntensity);
    }
}

void UAttachmentPoint::OnHoverBegin_Implementation()
{
    if (IsAvailable())
    {
       SetHighlighted(true);
       UE_LOG(LogTemp, Log, TEXT("Hovering attachment point: %s"), *GetName());
    }
}

void UAttachmentPoint::OnHoverEnd_Implementation()
{
    SetHighlighted(false);
}

void UAttachmentPoint::OnClicked_Implementation()
{
    // Not directly clickable
}
