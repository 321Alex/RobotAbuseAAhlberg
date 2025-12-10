#pragma once

#include "IAttachable.h"
#include "IClickable.h"
#include "IDraggable.h"
#include "IHoverable.h"
#include "GameFramework/Actor.h"
#include "AttachablePart.generated.h"

class UAttachmentPoint;

UENUM(BlueprintType)
enum class EPartState : uint8
{
    ATTACHED,
    HELD,
    DETACHED
};

UENUM(BlueprintType)
enum class EArmType : uint8
{
    Left,
    Right,
    Universal
};

UCLASS()
class ROBOTABUSE_API AAttachablePart : public AActor, public IClickable, public IHoverable, public IDraggable, public IAttachable
{
    GENERATED_BODY()
    
public:    
    AAttachablePart();

    virtual void BeginPlay() override;

    // IInteractable
    virtual void OnHoverBegin_Implementation() override;
    virtual void OnHoverEnd_Implementation() override;
    virtual void OnClicked_Implementation() override;
    
    // IDraggable
    virtual void UpdateDragPosition_Implementation(const FVector& WorldPosition) override;
    virtual void OnDropped_Implementation() override;
    
    // IAttachable
    virtual bool TryAttachTo_Implementation(UAttachmentPoint* Point) override;

    // Player actions
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void PickUp();

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void Drop();

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void AttachToPoint(UAttachmentPoint* Point);

    // State
    UFUNCTION(BlueprintPure, Category = "State")
    bool IsAttached() const { return CurrentState == EPartState::ATTACHED; }

    UFUNCTION(BlueprintPure, Category = "State")
    bool IsHeld() const { return CurrentState == EPartState::HELD; }

    UFUNCTION(BlueprintPure, Category = "State")
    UAttachmentPoint* GetAttachmentPoint() const { return CurrentAttachmentPoint; }
    
    UFUNCTION(BlueprintCallable, Category = "Attachment")
    void DetachFromPoint();
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arm Type")
    EArmType ArmType = EArmType::Universal;
    
    UPROPERTY(BlueprintReadOnly, Category = "State")
    EPartState CurrentState;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComponent;
    
    // This function is used by the UI to get correct text based on state
    UFUNCTION(BlueprintCallable, Category = "Part State")
    FString GetStateAsString() const
    {
        switch (CurrentState)
        {
        case EPartState::DETACHED: return TEXT("Detached");
        case EPartState::HELD: return TEXT("Being Dragged");
        case EPartState::ATTACHED: return TEXT("Attached");
        default: return TEXT("Unknown");
        }
    }

    UPROPERTY(BlueprintReadOnly, Category = "State")
    UAttachmentPoint* CurrentAttachmentPoint;

    // Visual config
    UPROPERTY(EditAnywhere, Category = "Visual")
    FName EmissiveParameterName = "EmissiveStrength";
    
    UPROPERTY(EditAnywhere, Category = "Interaction")
    FVector HeldOffset = FVector(-8.7f, 0.0f, 19.0f);

    UPROPERTY(EditAnywhere, Category = "Visual")
    float NormalEmissive = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Visual")
    float HighlightEmissive = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Visual")
    float HeldEmissive = 2.0f;

private:
    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> DynamicMaterials;

    void SetEmissive(float Value);
    void SetupMaterials();
};
