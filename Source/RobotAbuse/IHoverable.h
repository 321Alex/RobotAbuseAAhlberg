#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IHoverable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UHoverable : public UInterface
{
	GENERATED_BODY()
};

class ROBOTABUSE_API IHoverable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnHoverBegin();
    
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnHoverEnd();
};
