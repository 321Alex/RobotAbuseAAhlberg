#include "RobotSpectatorPawn.h"
#include "AttachablePart.h"
#include "AttachmentPoint.h"
#include "IClickable.h"
#include "IHoverable.h"
#include "IDraggable.h"
#include "IAttachable.h"
#include "Blueprint/UserWidget.h"

void ARobotSpectatorPawn::BeginPlay()
{
	Super::BeginPlay();

	CachedPC = Cast<APlayerController>(GetController());
	check(CachedPC);

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
	if (!CachedPC) return;

	FHitResult Hit;
	CachedPC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	UE_LOG(LogTemp, Display, TEXT("Click Test"));

	// If already dragging something
	if (DraggedActor)
	{
		// Check if we clicked on an attachment point
		USceneComponent* HitComponent = Hit.GetComponent();
		USceneComponent* Parent = HitComponent ? HitComponent->GetAttachParent() : nullptr;
		UAttachmentPoint* Point = Cast<UAttachmentPoint>(Parent);

		if (Point)
		{
			// Try to attach - let the object handle the logic
			if (DraggedActor->Implements<UAttachable>())
			{
				bool bAttached = IAttachable::Execute_TryAttachTo(DraggedActor, Point);

				if (bAttached)
				{
					// Success - stop dragging and broadcast event
					if (AAttachablePart* Part = Cast<AAttachablePart>(DraggedActor))
					{
						FString Message = FString::Printf(TEXT("Attached %s"), *Part->GetName());
						OnPartStateChanged.Broadcast(Part);
					}

					DraggedActor = nullptr;
					InitialDragDistance = 0.0f;
				}
				// If failed, keep dragging (wrong socket type)
			}
		} 
		else
		{
			// Clicked empty space - drop
			if (DraggedActor->Implements<UDraggable>())
			{
				IDraggable::Execute_OnDropped(DraggedActor);

				// Broadcast UI event, only updates if the current thing dragging is part 
				if (AAttachablePart* Part = Cast<AAttachablePart>(DraggedActor))
				{
					FString Message = FString::Printf(TEXT("Dropped %s"), *Part->GetName());
					OnPartStateChanged.Broadcast(Part);
				}
			}

			DraggedActor = nullptr;
			InitialDragDistance = 0.0f;
		}
	}
	// Not dragging, try to pick up
	else if (AActor* HitActor = Hit.GetActor())
	{
		HandleNewClick(HitActor);
	}
}

// = Interaction Functions =

void ARobotSpectatorPawn::HandleNewClick(AActor* Actor)
{
	if (Actor->Implements<UClickable>())
	{
		// Call click interface - part handles pickup itself via OnClicked
		IClickable::Execute_OnClicked(Actor);

		// Calculate drag distance from camera to object
		FVector CameraLocation;
		FRotator CameraRotation;
		CachedPC->GetPlayerViewPoint(CameraLocation, CameraRotation);
		float Distance = FVector::Dist(CameraLocation, Actor->GetActorLocation());

		// Start dragging
		DraggedActor = Actor;
		InitialDragDistance = Distance;

		// Broadcast UI event
		if (AAttachablePart* Part = Cast<AAttachablePart>(Actor))
		{
			FString Message = FString::Printf(TEXT("Picked up %s"), *Part->GetName());
			OnPartStateChanged.Broadcast(Part);
		}
	}
}

void ARobotSpectatorPawn::StopDragging()
{
	if (DraggedActor)
	{
		UE_LOG(LogTemp, Log, TEXT("Stopped dragging: %s"), *DraggedActor->GetName());
	}

	DraggedActor = nullptr;
}

// ===== Update Functions =====

void ARobotSpectatorPawn::UpdateDraggedActor()
{
	if (!CachedPC || !DraggedActor) return;

	// Get the world position and direction from the mouse cursor
	FVector MouseWorldLocation;
	FVector MouseWorldDirection;

	if (!CachedPC->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to deproject mouse position"));
		return;
	}

	// Calculate where the dragged object should be:
	// Start at mouse world position, extend along direction by the initial distance
	FVector NewLocation = MouseWorldLocation + (MouseWorldDirection * InitialDragDistance);

	// Tell the dragged object to update its position via interface
	if (DraggedActor->Implements<UDraggable>())
	{
		IDraggable::Execute_UpdateDragPosition(DraggedActor, NewLocation);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DraggedActor does not implement IDraggable!"));
	}
}

void ARobotSpectatorPawn::UpdateHighlights()
{
	if (!CachedPC) return;

	FHitResult Hit;
	CachedPC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

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
			OnPartStateChanged.Broadcast(nullptr); // Cleared hover
		}
	}
}
