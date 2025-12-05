#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IClickable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UClickable : public UInterface
{
	GENERATED_BODY()
};

class ROBOTABUSE_API IClickable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnClicked();
};
