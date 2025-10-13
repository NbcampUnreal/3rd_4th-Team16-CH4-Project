// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/POSelectBoxWidget.h"

#include "Components/Button.h"
#include "Controllers/Components/POUIStackingComponent.h"
#include "Controllers/Interfaces/POUIStackingInterface.h"

void UPOSelectBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (YesButton && !YesButton->OnClicked.IsAlreadyBound(this, &UPOSelectBoxWidget::OnYesButtonClicked))
	{
		YesButton->OnClicked.AddDynamic(this, &UPOSelectBoxWidget::OnYesButtonClicked);
	}
	if (NoButton && !NoButton->OnClicked.IsAlreadyBound(this, &UPOSelectBoxWidget::OnNoButtonClicked))
	{
		NoButton->OnClicked.AddDynamic(this, &UPOSelectBoxWidget::OnNoButtonClicked);
	}
}

void UPOSelectBoxWidget::NativeDestruct()
{
	if (YesButton && YesButton->OnClicked.IsAlreadyBound(this, &UPOSelectBoxWidget::OnYesButtonClicked))
	{
		YesButton->OnClicked.RemoveDynamic(this, &UPOSelectBoxWidget::OnYesButtonClicked);
	}
	if (NoButton && NoButton->OnClicked.IsAlreadyBound(this, &UPOSelectBoxWidget::OnNoButtonClicked))
	{
		NoButton->OnClicked.RemoveDynamic(this, &UPOSelectBoxWidget::OnNoButtonClicked);
	}
	
	Super::NativeDestruct();
}

void UPOSelectBoxWidget::OnYesButtonClicked()
{
	// TODO: 상속받은 클래스에서 구현
}

void UPOSelectBoxWidget::OnNoButtonClicked()
{
	if (IPOUIStackingInterface* UIStackingInterface = Cast<IPOUIStackingInterface>(GetOwningPlayer()))
	{
		UIStackingInterface->GetUIStackingComponent()->PopWidget();
	}
}
