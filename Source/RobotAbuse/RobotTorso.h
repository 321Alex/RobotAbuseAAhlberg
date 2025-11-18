#pragma once

#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "RobotTorso.generated.h"

UCLASS()
class ROBOTABUSE_API ARobotTorso : public AActor, public IInteractable
{
	GENERATED_BODY()
    
public:    
	ARobotTorso();

	virtual void BeginPlay() override;

	// IInteractable
	virtual void OnHoverBegin_Implementation() override;
	virtual void OnHoverEnd_Implementation() override;
	virtual void OnClicked_Implementation() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* RootMesh;

	// Visual config
	UPROPERTY(EditAnywhere, Category = "Visual")
	FName EmissiveParameterName = "EmissiveStrength";

	UPROPERTY(EditAnywhere, Category = "Visual")
	float NormalEmissive = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Visual")
	float HighlightEmissive = 3.0f;

private:
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;

	void SetupMaterials();
	void SetEmissive(float Value);
	void SetEmissiveIncludingAttachedParts(float Value);
};