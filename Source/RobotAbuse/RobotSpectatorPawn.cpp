#include "RobotSpectatorPawn.h"
#include "AttachablePart.h"
#include "AttachmentPoint.h"
#include "Blueprint/UserWidget.h"

void ARobotSpectatorPawn::BeginPlay()
{
    Super::BeginPlay();
    
    //This condition makes sure that the pawn is player controlled
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UClass* WidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/Widgets/WBP_ArmStatus.WBP_ArmStatus_C")))
        {
            UUserWidget* Widget = CreateWidget<UUserWidget>(PC, WidgetClass);
            if (Widget)
            {
                Widget->AddToViewport();
            }
        }
        
        PC->bShowMouseCursor = true;
        PC->bEnableClickEvents = true;
        PC->bEnableMouseOverEvents = true;
    }
}

void ARobotSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Parent class already handles movement input
    // Added custom interaction
    PlayerInputComponent->BindAction("MouseClick", IE_Pressed, this, &ARobotSpectatorPawn::OnMouseClick);
}

void ARobotSpectatorPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (DraggedActor)
    {
        UpdateDraggedActor();
    }
    
    UpdateHighlights();
}

void ARobotSpectatorPawn::OnMouseClick()
{
    // TODO: Extract helper functions to improve readability (e.g., TryAttachPart, HandlePartDrop)
    
    APlayerController* PC = Cast<APlayerController>(GetController());
    
    // Raycast to determine what the mouse is clicking
    FHitResult Hit;
    PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
    
    // Handle release if currently dragging an object
    if (DraggedActor)
    {
        // Check if we're releasing over a valid attachment point
        if (DraggedPart)
        {
            UAttachmentPoint* Point = nullptr;

            USceneComponent* HitComponent = Hit.GetComponent();
            if (HitComponent)
            {
                // NOTE: Attachment point uses a scene component, collision is on child sphere mesh.
                // Would refactor to use a dedicated collision component for cleaner design.
                USceneComponent* Parent = HitComponent->GetAttachParent();
                if (Parent)
                {
                    Point = Cast<UAttachmentPoint>(Parent);
                }
            }
            
            if (Point && Point->CanAcceptPart(DraggedPart))
            {
                // Tell the point about the attachment (hides visual)
                Point->AttachPart(DraggedPart);
    
                // Tell the part about the attachment (updates state)
                DraggedPart->AttachToPoint(Point);
    
                UE_LOG(LogTemp, Log, TEXT("Attached part to %s"), *Point->GetName());
            }
            else
            {
                // TODO: Add player feedback (sound, UI message) for incompatible socket
                if (Point)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Wrong socket type - continuing drag"));
                    return;
                }
                else
                {
                    // Not near any socket - drop it
                    DraggedPart->Drop();
                    UE_LOG(LogTemp, Log, TEXT("Dropped part"));
                }
            }
            OnPartStateChanged.Broadcast(DraggedPart);
        }
        
        StopDragging();
    }
    else
    {
        //Begin dragging new actor
        if (Hit.GetActor() && Hit.GetActor()->Implements<UClickable>())
        {
            IClickable::Execute_OnClicked(Hit.GetActor());
            StartDragging(Hit.GetActor());
        }
    }
}

// = Interaction Functions =

void ARobotSpectatorPawn::StartDragging(AActor* Actor)
{
    DraggedActor = Actor;
    
    // Calculate initial distance from camera
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        FVector CameraLocation;
        FRotator CameraRotation;
        PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
        
        InitialDragDistance = FVector::Dist(CameraLocation, Actor->GetActorLocation());
        
        UE_LOG(LogTemp, Log, TEXT("Initial drag distance: %f"), InitialDragDistance);
    }
    
    // Check if it's an attachable part
    if (AAttachablePart* Part = Cast<AAttachablePart>(Actor))
    {
        DraggedPart = Part;
        Part->PickUp();
        
        // Broadcast UI event
        OnPartStateChanged.Broadcast(Part);
        
        UE_LOG(LogTemp, Log, TEXT("Started dragging part: %s"), *Actor->GetName());
    }
    else
    {
        DraggedPart = nullptr;
        UE_LOG(LogTemp, Log, TEXT("Started dragging actor: %s"), *Actor->GetName());
    }
}

void ARobotSpectatorPawn::StopDragging()
{
    if (DraggedActor)
    {
        UE_LOG(LogTemp, Log, TEXT("Stopped dragging: %s"), *DraggedActor->GetName());
    }
    
    DraggedActor = nullptr;
    DraggedPart = nullptr;
}

// ===== Update Functions =====

void ARobotSpectatorPawn::UpdateDraggedActor()
{
    if (!DraggedActor) return;
    
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;
    
    // Get mouse position
    float MouseX, MouseY;
    if (!PC->GetMousePosition(MouseX, MouseY))
        return;
    
    // Get camera
    FVector CameraLocation;
    FRotator CameraRotation;
    PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
    
    // Convert mouse to world ray
    FVector WorldLocation, WorldDirection;
    PC->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);
    
    // Use the initial distance we stored
    FVector NewLocation = CameraLocation + (WorldDirection * InitialDragDistance);
    
    //TODO Make a drag component and add a interface
    if (DraggedPart)
    {
        DraggedPart->UpdateHeldPosition(NewLocation);
    }
    else
    {
        DraggedActor->SetActorLocation(NewLocation);
    }
}

void ARobotSpectatorPawn::UpdateHighlights()
{
    //TODO this should be a saved reference
    APlayerController* PC = Cast<APlayerController>(GetController());

    FHitResult Hit;
    PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
    
    AActor* NewTarget = nullptr;
    
    if (!DraggedActor)
    {
        if (Hit.GetActor() && Hit.GetActor()->Implements<UClickable>())
        {
            NewTarget = Hit.GetActor();
        }
    }
    
    // Update highlight if changed
    if (NewTarget != HoveredTarget)
    {
        // End hover on old target
        if (HoveredTarget && HoveredTarget->Implements<UHoverable>())
        {
            IHoverable::Execute_OnHoverEnd(HoveredTarget);
        }
        
        // Begin hover on new target
        if (NewTarget && NewTarget->Implements<UHoverable>())
        {
            IHoverable::Execute_OnHoverBegin(NewTarget);
        }
        
        HoveredTarget = NewTarget;
        
        if (AAttachablePart* Part = Cast<AAttachablePart>(NewTarget))
        {
            OnPartStateChanged.Broadcast(Part);
        }
        else
        {
            OnPartStateChanged.Broadcast(nullptr);  // Cleared hover
        }
    }
}

void ARobotSpectatorPawn::SetHighlight(UObject* Target, bool bHighlight)
{
    if (!Target) return;
    
    if (Target->Implements<UInteractable>())
    {
        if (bHighlight)
        {
            IInteractable::Execute_OnHoverBegin(Target);
        }
        else
        {
            IInteractable::Execute_OnHoverEnd(Target);
        }
    }
}
