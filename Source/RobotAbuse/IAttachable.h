#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IAttachable.generated.h"

// Forward declaration
class UAttachmentPoint;

UINTERFACE(MinimalAPI, Blueprintable)
class UAttachable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for objects that can attach to attachment points
 */
class ROBOTABUSE_API IAttachable
{
	GENERATED_BODY()

public:
	/**
	 * Attempt to attach this object to an attachment point
	 * Object is responsible for checking compatibility and handling attachment
	 * @param Point The attachment point to try attaching to
	 * @return True if successfully attached, false if attachment failed
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attachment")
	bool TryAttachTo(UAttachmentPoint* Point);
};