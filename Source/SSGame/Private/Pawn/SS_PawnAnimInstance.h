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

private:

	USS_GameInstance* GInstance;
	ASS_Pawn* OwningPawn;

	//

	ESS_PawnState CurrentState;
};
