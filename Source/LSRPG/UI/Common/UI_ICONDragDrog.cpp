// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_ICONDragDrog.h"
#include "Components/Image.h"

void UUI_ICONDragDrog::DrawICON(UTexture2D* InICON)
{
	ICON->SetBrushFromTexture(InICON);
}
