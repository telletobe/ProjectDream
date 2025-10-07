// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectDreamCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ProjectDreamPlayerController.h"
#include "../GameSystems/Inventory/GameInventory.h"
#include "../GameSystems/Inventory/GameItem.h"
#include "GameSystems/DreamGameInstance.h"
#include "GameSystems/DreamGameInstanceSubsystem.h"
#include "GameSystems/Achievements/AchievementsSubsystem.h"
#include "GameSystems/Achievements/AchievementsManager.h"
#include "GameSystems/Common/GameEventBus/GameEventBus.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AProjectDreamCharacter::AProjectDreamCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AProjectDreamCharacter::BeginPlay()
{
	Super::BeginPlay();

	AProjectDreamPlayerController* PC = Cast<AProjectDreamPlayerController>(GetController());

	FInputModeGameAndUI InputMode;

	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);

	if (PC)
	{
		PC->SetShowMouseCursor(true);
		PC->SetIgnoreLookInput(true);
		PC->SetInputMode(InputMode);
	}

	if (!ItemInventory)
	{
		ItemInventory = NewObject<UGameInventory>(this, UGameInventory::StaticClass());
		if (ItemInventory)
		{
			ItemInventory->SetOwner(this);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::MakeRandomColor(), TEXT("Inventory Create"));
			UE_LOG(LogTemplateCharacter,Warning,TEXT("Inventory Create"));
			OnReadyInventory.Broadcast(ItemInventory);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::MakeRandomColor(), TEXT("Inventory BroadCast"));
		}
	}

	//if (UDreamGameInstance* GI = Cast<UDreamGameInstance>(GetGameInstance()))
	//{
	//	if (UDreamGameInstanceSubsystem* SaveSys = GI->GetSubsystem<UDreamGameInstanceSubsystem>())
	//	{
	//		SaveSys->LoadInventoryJson(TEXT("TestSave"));
	//		UE_LOG(LogTemp, Warning, TEXT("GameInstance Valid"));
	//	}
	//}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAchievementsSubsystem* AchieveSubSystem = GI->GetSubsystem<UAchievementsSubsystem>())
		{
			if (UGameEventBus* EventBus = GI->GetSubsystem<UGameEventBus>())
			{

				EventBus->Publish(FName(TEXT("GAME_START")));
				EventBus->Publish(FName(TEXT("START_TEST")));
				UE_LOG(LogTemp, Warning, TEXT("GAME_START Achieve BroadCast Call"));
			}
		}	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance inValid"));
	}

}

void AProjectDreamCharacter::SetInteractTarger(AGameItem* Target)
{
	Focused = Target;
}

void AProjectDreamCharacter::ClearInteractTarget(UObject* Who)
{
	if (Focused == Who) Focused = nullptr;
}

void AProjectDreamCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProjectDreamCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AProjectDreamCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProjectDreamCharacter::Look);

		// Open Inventory
		EnhancedInputComponent->BindAction(InventoryEventAction, ETriggerEvent::Triggered, this, &AProjectDreamCharacter::OpenInventory);
		
		// Interact
		EnhancedInputComponent->BindAction(InterationAction, ETriggerEvent::Triggered, this, &AProjectDreamCharacter::OnInteract);

		// Achievement
		EnhancedInputComponent->BindAction(AchievementAction, ETriggerEvent::Triggered, this, &AProjectDreamCharacter::OnAchievement);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AProjectDreamCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AProjectDreamCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AProjectDreamCharacter::OpenInventory(const FInputActionValue& Input)
{
	// BroadCast();
	GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::MakeRandomColor(), TEXT("OpenInventory Event"));
	UE_LOG(LogTemp,Warning,TEXT("OpenInventory Event"));
	OnInventoryEvent.Broadcast();
}

void AProjectDreamCharacter::OnInteract(const FInputActionValue& Value)
{
	if (Focused)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::MakeRandomColor(), TEXT("OnInteractAction Event"));
		UE_LOG(LogTemp, Warning, TEXT("OnInteractAction Event"));
		OnInteractAction.Broadcast(this);
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UGameEventBus* EventBus = GI->GetSubsystem<UGameEventBus>())
			{

				EventBus->Publish(FName(TEXT("TEST_ACHIEVEMENT")));
				UE_LOG(LogTemp, Warning, TEXT("TEST_ACHIEVEMENT Achieve BroadCast Call"));
			}
		}
	}
}

void AProjectDreamCharacter::OnAchievement(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Call AchievementEvent"));
	OnAchievementEvent.Broadcast();
	return;
}

void AProjectDreamCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AProjectDreamCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AProjectDreamCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AProjectDreamCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}
