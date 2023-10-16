// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataAnalyticsBPLibrary.h"
#include "DataAnalytics.h"

UDataAnalyticsBPLibrary::UDataAnalyticsBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UDataAnalyticsBPLibrary::DataToStringImpl(FProperty* prop, void* structPtr)
{
	ResetValues();
	
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Red, *prop->GetAuthoredName());
	
	FStructProperty* StructProperty = CastField<FStructProperty>(prop);
	if(StructProperty)
	{
		//Go trough Struct
		for (TFieldIterator<FProperty> PropertyIt(StructProperty->Struct); PropertyIt; ++PropertyIt)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Red, *PropertyIt->GetAuthoredName());
			HeaderNames.Append(*PropertyIt->GetAuthoredName());
		}
	}
}

void UDataAnalyticsBPLibrary::ResetValues()
{
	HeaderNames.Empty();
}

FString UDataAnalyticsBPLibrary::GetHeaderNames()
{
	return HeaderNames;
}


