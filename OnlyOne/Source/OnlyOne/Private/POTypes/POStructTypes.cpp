// Fill out your copyright notice in the Description page of Project Settings.


#include "POTypes/POStructTypes.h"

bool FPOInputActionConfig::IsValid() const
{
	return InputTag.IsValid() && InputAction;
}
