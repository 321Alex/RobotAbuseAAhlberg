#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDraggable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UDraggable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for objects that can be dragged in 3D space
 */
class ROBOTABUSE_API IDraggable
{
	GENERATED_BODY()

public:
	/**
	 * Called when the object should update its position while being dragged
	 * @param WorldPosition The target world position from the drag system
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dragging")
	void UpdateDragPosition(const FVector& WorldPosition);
    
	/**
	 * Called when the object is dropped (released while dragging)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dragging")
	void OnDropped();
};
