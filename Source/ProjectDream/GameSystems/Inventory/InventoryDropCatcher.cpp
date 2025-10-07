// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Inventory/InventoryDropCatcher.h"
#include "InventoryDropCatcher.h"
#include "Gamesystems/Inventory/GameInventory.h"
#include "Blueprint/DragDropOperation.h"
#include "GameFramework/Character.h"     


void UInventoryDropCatcher::InitCatcher(UGameInventory* InInventory, APlayerController* InPC)
{
	InventoryRef = InInventory;
	PCRef = InPC;
}

void UInventoryDropCatcher::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Visible);
}

bool UInventoryDropCatcher::NativeOnDrop(const FGeometry& InGeo, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOp)
{
	if (InOp && InOp->Payload)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::MakeRandomColor(), TEXT("Call Drag NativeOnDrop"));
		const FVector2D ScreenPos = InDragDropEvent.GetScreenSpacePosition();

		SetVisibility(ESlateVisibility::Collapsed);
		return true;

	}

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::MakeRandomColor(), TEXT("Call Drag NativeOnDrop"));
	SetVisibility(ESlateVisibility::Collapsed);
	return false;

}

void UInventoryDropCatcher::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOp)
{
	GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::MakeRandomColor(),TEXT("Call Drag cancelled"));
	SetVisibility(ESlateVisibility::Collapsed);
	Super::NativeOnDragCancelled(InDragDropEvent,InOp);
}

void UInventoryDropCatcher::HandleDropToWorld(const FVector2D& ScreenPos, UObject* Payload)
{
	if (!PCRef.IsValid() || !InventoryRef.IsValid()) return;

	// (선택) 수량 선택 모달을 열고 결과로 n개 버리기 등…
	// 여기서는 단일 개체 드랍 예시

	// 드랍한 위치로 월드 스폰을 원한다면 화면좌표→월드 레이 만들기
	FVector WorldOrigin, WorldDir;
	if (PCRef->DeprojectScreenPositionToWorld(ScreenPos.X, ScreenPos.Y, WorldOrigin, WorldDir))
	{
		const FVector SpawnLoc = PCRef->GetCharacter()->GetActorLocation();
		UWorld* World = GetWorld();
		if (!World) return;

		
		//World->SpawnActor<AGameItem>();


		// ① 인벤토리에서 아이템 제거
		//    (Payload에서 아이템 ID/Index를 얻어 제거)
		// InventoryRef->RemoveAt(Index) 등 사용자 함수 호출

		// ② 아이템 액터 스폰(사용자 프로젝트의 아이템 액터 클래스로 교체)
		// World->SpawnActor<AGameItemActor>(ItemClass, SpawnLoc, FRotator::ZeroRotator);

		// ③ 필요하면 인벤토리 UI 갱신 브로드캐스트
		// InventoryRef->ChangeInventoryData.Broadcast();
	}
}


