#pragma once

#include "CoreMinimal.h"
#include "AttachablePart.h"
#include "Components/SceneComponent.h"
#include "Interactable.h"
#include "AttachmentPoint.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class AAttachablePart;
class UChildActorComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ROBOTABUSE_API UAttachmentPoint : public USceneComponent, public IInteractable
{
    GENERATED_BODY()

public:
    UAttachmentPoint();

protected:
    virtual void BeginPlay() override;
    virtual void OnRegister() override; // Called when component is registered in editor

public:
    // Material parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment|Material")
    FName EmissiveParameterName = "EmissiveIntensity";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment|Material")
    float NormalIntensity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment|Material")
    float HighlightIntensity = 5.0f;

    // Reference to a Child Actor Component that contains the arm
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment|Setup")
    UChildActorComponent* InitialAttachedArmComponent;

    UPROPERTY(BlueprintReadOnly, Category = "Attachment")
    AAttachablePart* AttachedPart;

    // Check if available for attachment
    UFUNCTION(BlueprintPure, Category = "Attachment")
    bool IsAvailable() const { return AttachedPart == nullptr; }

    // Attachment functions
    UFUNCTION(BlueprintCallable, Category = "Attachment")
    void AttachPart(AAttachablePart* Part);

    UFUNCTION(BlueprintCallable, Category = "Attachment")
    void DetachPart();

    // Visual functions
    UFUNCTION(BlueprintCallable, Category = "Attachment")
    void ShowAttachmentVisual(bool bShow);

    UFUNCTION(BlueprintCallable, Category = "Attachment")
    void SetHighlighted(bool bHighlight);
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
    EArmType AcceptedArmType = EArmType::Universal;

    // Helper function
    UFUNCTION(BlueprintCallable, Category = "Attachment")
    bool CanAcceptPart(AAttachablePart* Part) const;

    // IInteractable interface
    virtual void OnHoverBegin_Implementation() override;
    virtual void OnHoverEnd_Implementation() override;
    virtual void OnClicked_Implementation() override;

private:
    UPROPERTY()
    UStaticMeshComponent* AttachmentVisual;

    UPROPERTY()
    UMaterialInstanceDynamic* VisualMaterial;

    void FindAttachmentVisual();
    void SetupVisual();
    void RegisterInitialPart();
};