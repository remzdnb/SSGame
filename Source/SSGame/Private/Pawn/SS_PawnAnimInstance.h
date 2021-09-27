#pragma once

#include "SSGame.h"
#include "Animation/AnimInstance.h"
#include "SS_PawnAnimInstance.generated.h"

class USS_GameInstance;
class ASS_Pawn;

UCLASS()
class USS_PawnAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	//

	UFUNCTION()
	void PlayIdleAnimation();

	UFUNCTION()
	void PlayMoveStartAnimation();

	UFUNCTION()
	void PlayMoveEndAnimation();

	UFUNCTION()
	void PlayAttackStartAnimation();

	UFUNCTION()
	void PlayAttackEndAnimation();

private:

	USS_GameInstance* GInstance;
	ASS_Pawn* OwningPawn;

	//

	ESS_PawnAnimation LastAnimation;
};
