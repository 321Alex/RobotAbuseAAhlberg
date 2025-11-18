#pragma once

#include "UObject/Interface.h"
#include "Interactable.generated.h"

//Creating new type of interface
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for interactable objects in the world
 */
class IInteractable
{
	GENERATED_BODY()

public:
	// Visual feedback
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnHoverBegin();
    
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnHoverEnd();
    
	// Main interaction
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnClicked();
};
