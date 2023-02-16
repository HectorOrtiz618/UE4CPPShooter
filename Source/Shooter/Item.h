// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;
class UWidgetComponent;
class USphereComponent;
class UCurveFloat;
class AShooterCharacter;
class USoundCue;
class UCurveVector;
class UDataTable;

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damanged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_Uncommon UMETA(DisplayName = "Uncommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayName = "Pick Up"),
	EIS_EquipInterping UMETA(DisplayName = "Equip Interping"),
	EIS_PickedUp UMETA(DisplayName = "Picked Up"),
	EIS_Equipped UMETA(DisplayName = "Eqipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),
	
	EIS_MAX UMETA(DisplayName = "DefaultMAX")
};
UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Weapon UMETA(DisplayName = "Weapon"),

	EIT_MAX UMETA(DisplayName = "Default MAX"),
};
USTRUCT(BlueprintType)
struct FItemRarityTable :public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadwrite)
	FLinearColor GlowColor;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	FLinearColor LightColor;
	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	FLinearColor DarkColor;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	int32 NumberOfStars;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	UTexture2D* IconBackground;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	int32 CustomDepthStencil;

};
UCLASS()
class SHOOTER_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();	
	
private:
	//Skeletal Mesh for Item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	//Line traces collides with box to show HUD widgets
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox;

	//Pop up widget for when player looks at item
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	UWidgetComponent* PickupWidget;

	//Enables Item tracing when overlapped
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	FString ItemName;

	//Item Count i.e. Ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	int32 ItemCount;

	//ItemRareity determines number of stars in Widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rarity", META = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

	//State of Item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	EItemState ItemState;

	//Curve for Z axis when in interp state
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemZCurve;

	//Starting location when inbterping begins
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	FVector ItemInterpStartLocation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	FVector CameraTargetLocation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	bool bInterping;
	
	FTimerHandle ItemInterpTimer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	AShooterCharacter* Character;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	float ZCurveTime;

	float ItemInterpX;
	float ItemInterpY;

	float InitialYawOffset;

	//Curve used to scale item when interping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemScaleCurve;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Item Properties", META =(AllowPrivateAccess = "true"))
	USoundCue* PickupSound;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	USoundCue* EquipSound;
	
	//Enum for the type of item we're picking up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	EItemType ItemType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	int32 InterpLocationIndex;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	int32 MaterialIndex;


	//Dynamic Instance that we can change at run time
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DyanamicMaterialInstance;
	
	//Material Instance used with DynamicMaterialInstance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	UMaterialInstance* MaterialInstance;

	bool bCanChangeCustomDepth;

	//Curve to drive the dynamic material parameters
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	UCurveVector* PulseCurve;

	//Curve to drive the dynamic material pulse curve when interping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	UCurveVector* InterpPulseCurve;

	FTimerHandle PulseTimer;

	//Time for Pulse Timer
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	float PulseCurveTime;

	UPROPERTY(VisibleAnywhere, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	float GlowAmount;
	
	UPROPERTY(VisibleAnywhere, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	float FresnelExponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Item Properties", META = (AllowPrivateAccess = "true"))
	float FresnelReflectFraction;

	//Icon of the Weapon used for the Inventory in HUD
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", META = (AllowPrivateAccess = "true"))
	UTexture2D* WeaponIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", META = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIcon;

	//Slot In Inventory Array
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Inventory", META = (AllowPrivateAccess = "true"))
	int32 SlotIndex;
	//True when player's inventory is full
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", META = (AllowPrivateAccess = "true"))
	bool bCharacterInventoryFull;

	//Item Rareity Table
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "DataTable", META = (AllowPrivateAccess = "true"))
	UDataTable* ItemRarityDataTable;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Rarity", Meta = (AllowPrivateAccess = "true"))
	FLinearColor GlowColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rarity", Meta = (AllowPrivateAccess = "true"))
	FLinearColor LightColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rarity", Meta = (AllowPrivateAccess = "true"))
	FLinearColor DarkColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rarity", Meta = (AllowPrivateAccess = "true"))
	int32 NumberOfStars;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rarity", Meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconBackground;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//UFUCNTION() macros need for binding functions to overlap events
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetActiveStars();
	//SetItem properties based on state, overriding for setting StaticMesh settings

	virtual void SetItemProperties();
	void FinishInterping();
	//Handles Item Interp
	void ItemInterp(float DeltaTime);

	FVector GetInterpLocation();

	void PlayPickupSound(bool bForcePlay = false);

	virtual void InitializeCustomDepth();

	virtual void OnConstruction(const FTransform& Transform) override;

	void EnableGlowMaterial();
	void UpdatePulse();

	void StartPulseTimer();
	void ResetPulseTimer();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE UWidgetComponent* GetWidgetComponent() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }

	FORCEINLINE USkeletalMeshComponent* GetItemMesh()const { return ItemMesh; }

	FORCEINLINE EItemState GetItemState() const{ return ItemState; }
	
	FORCEINLINE USoundCue* GetPickupSound() const{ return PickupSound; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }

	FORCEINLINE void SetPickupSound(USoundCue* NewPickupSound) { PickupSound = NewPickupSound; }
	FORCEINLINE void SetEquipSound(USoundCue* NewEquipSound) { EquipSound = NewEquipSound; }
	
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }
	
	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE void SetSlotIndex(int32 Index) { SlotIndex = Index; }
	FORCEINLINE void SetCharacter(AShooterCharacter* ASC) { Character = ASC; };

	FORCEINLINE void SetCharacterInventoryFull(bool bFull) { bCharacterInventoryFull = bFull; }

	FORCEINLINE void SetItemName(FString Name) { ItemName = Name; }
	
	FORCEINLINE void SetIconItem(UTexture2D* Icon) { WeaponIcon = Icon; }
	FORCEINLINE void SetAmmoIcon(UTexture2D* Icon) { AmmoIcon = Icon; }

	FORCEINLINE void SetMaterialInstance(UMaterialInstance* Instance) { MaterialInstance = Instance; };
	FORCEINLINE UMaterialInstance* GetMaterialInstance() const{ return MaterialInstance; }
	FORCEINLINE void SetDynamicMaterialInstance(UMaterialInstanceDynamic* Instance) { DyanamicMaterialInstance = Instance; };
	FORCEINLINE UMaterialInstanceDynamic* GetDynamicMaterialInstance() const{ return DyanamicMaterialInstance; }

	FORCEINLINE FLinearColor GetGlowColor()const { return GlowColor; }
	FORCEINLINE int32 GetMaterialIndex() const { return MaterialIndex; }
	FORCEINLINE void SetMaterialIndex(int32 Index) { MaterialIndex = Index; }

	void PLayEquipSound(bool bForcePlay = false);
	void DisableGlowMaterial();

	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();
	
	void SetItemState(EItemState NewState);
	void StartItemCurve(AShooterCharacter* Char, bool bForcePlay = false);

};
