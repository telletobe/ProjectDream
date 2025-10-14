// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DreamItemDTO.h"
#include "GameFramework/Actor.h"
#include "GameItem.generated.h"

class ACharacter;

UCLASS()
class PROJECTDREAM_API AGameItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameItem();
	int32 GetItemID() const { return ItemID; }
	EItemCategory GetItemCategory() const { return ItemType; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()	void Interact(ACharacter* Interactor);

protected:
	UFUNCTION()
	void OnBoxBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	UPROPERTY(EditAnywhere) TObjectPtr<class UBoxComponent> BoxCollision;
	UPROPERTY(EditAnywhere)	TObjectPtr<class UStaticMeshComponent> BoxMesh;
	UPROPERTY(EditAnywhere, Category="Item") int32 ItemID =-1;
	UPROPERTY(EditAnywhere, Category = "Item") EItemCategory ItemType;
	UPROPERTY(EditAnywhere, Category = "Item") int32 CurrentItemCnt = 1;
	UPROPERTY()	TWeakObjectPtr<ACharacter> BoundCharacter;
};
