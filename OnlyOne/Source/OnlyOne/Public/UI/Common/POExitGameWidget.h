// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "POSelectBoxWidget.h"
#include "POExitGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONLYONE_API UPOExitGameWidget : public UPOSelectBoxWidget
{
	GENERATED_BODY()

protected:
	virtual void OnYesButtonClicked() override;
};
