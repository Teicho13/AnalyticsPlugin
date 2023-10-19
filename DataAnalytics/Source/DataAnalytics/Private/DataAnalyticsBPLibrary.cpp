// Copyright Epic Games, Inc. All Rights Reserved.
// UE_LOG(LogTemp, Warning, TEXT("This is the data: %s"), *FString(ReceivedData));

#include "DataAnalyticsBPLibrary.h"
#include "DataAnalytics.h"
#include "Misc/FileHelper.h"

UDataAnalyticsBPLibrary::UDataAnalyticsBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UDataAnalyticsBPLibrary::DataToStringImpl(FProperty* prop, void* structPtr, int layer)
{
	//Cast property to a struct
	FStructProperty* StructProperty = CastField<FStructProperty>(prop);

	//Check if empty / null
	if(StructProperty)
	{
		//Go trough Struct
		for (TFieldIterator<FProperty> PropertyIt(StructProperty->Struct); PropertyIt; ++PropertyIt)
		{
			//Record header name
			UE_LOG(LogTemp, Warning, TEXT("Header: %s"), *PropertyIt->GetAuthoredName());

			//Check value dimensions
			for (int i = 0; i < PropertyIt->ArrayDim; ++i)
			{
				//Get pointer to item in array
				void* ValuePtr = PropertyIt->ContainerPtrToValuePtr<void>(structPtr,i);

				//Parse item
				FString temp = ParseStructData(*PropertyIt, ValuePtr, layer);
			}
		}
	}

}

FString UDataAnalyticsBPLibrary::ParseStructData(FProperty* prop, void* valuePtr, int layer)
{
	//Supported Types
	
	float FloatValue;
	int32 IntValue;
	bool BoolValue;
	FString StringValue;
	FName NameValue;
	FText TextValue;
	FString TempString = "";

	//Check for integer / float
	if(FNumericProperty* NumericProperty = CastField<FNumericProperty>(prop))
	{
		//Is an integer
		if(NumericProperty->IsInteger())
		{
			IntValue = NumericProperty->GetSignedIntPropertyValue(valuePtr);
			TempString.AppendInt(IntValue);
			UE_LOG(LogTemp, Warning, TEXT("Parse: Is an Int"));
			return "";
		}
		//Is a float
		if(NumericProperty->IsFloatingPoint())
		{
			FloatValue = NumericProperty->GetFloatingPointPropertyValue(valuePtr);
			FString FloatStr = FString::SanitizeFloat(FloatValue);
			TempString.Append(FloatStr);
			UE_LOG(LogTemp, Warning, TEXT("Parse: Is a Float"));
			return "";
		}
	}
	//Check for boolean
	else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(prop))
	{
		BoolValue = BoolProperty->GetPropertyValue(valuePtr);
		if(BoolValue)
		{
			//true
			TempString.Append("True");
			UE_LOG(LogTemp, Warning, TEXT("Parse: Is a Bool (True)"));
			return "";
		}else
		{
			//false
			TempString.Append("False");
			UE_LOG(LogTemp, Warning, TEXT("Parse: Is a Bool (False)"));
			return "";
		}
	}
	//Check for names
	else if (FNameProperty* NameProperty = CastField<FNameProperty>(prop))
	{
		NameValue = NameProperty->GetPropertyValue(valuePtr);
		TempString.Append(NameValue.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Parse: Is a Name"));
		return "";
	}
	//Check for string
	else if (FStrProperty* StringProperty = CastField<FStrProperty>(prop))
	{
		StringValue = StringProperty->GetPropertyValue(valuePtr);
		TempString.Append(StringValue);
		UE_LOG(LogTemp, Warning, TEXT("Parse: Is a String"));
		return "";
	}
	//Check for text
	else if (FTextProperty* TextProperty = CastField<FTextProperty>(prop))
	{
		TextValue = TextProperty->GetPropertyValue(valuePtr);
		TempString.Append(TextValue.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Parse: Is a Text"));
		return "";
	}
	//Check for array
	else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(prop))
	{
		UE_LOG(LogTemp, Warning, TEXT("Parse: Is an Array"));

		//Get a helper to go trough array
		FScriptArrayHelper Helper(ArrayProperty,valuePtr);
		
		//Loop trough and parse all array values
		for (int i = 0, n = Helper.Num(); i < n; ++i)
		{
			ParseStructData(ArrayProperty->Inner,Helper.GetRawPtr(i),layer + 1);
		}

		return "";
	}
	// Reading a nested struct
	else if (prop)
	{
		UE_LOG(LogTemp, Warning, TEXT("Parse: Is a Struct"));

		//Go into struct again
		FStructProperty* StructProperty = CastField<FStructProperty>(prop);
		if(StructProperty)
		{
			for (TFieldIterator<FProperty> PropertyIt(StructProperty->Struct); PropertyIt; ++PropertyIt)
			{
				UE_LOG(LogTemp, Warning, TEXT("Sub Header: %s"), *PropertyIt->GetAuthoredName());
				//Check value dimensions
				for (int i = 0; i < PropertyIt->ArrayDim; ++i)
				{
					void* ValuePtr = PropertyIt->ContainerPtrToValuePtr<void>(valuePtr,i);

					FString temp = ParseStructData(*PropertyIt, ValuePtr, layer);
				}
			}
		}
		return "";
	}

	UE_LOG(LogTemp, Warning, TEXT("PARSE: NO RESULTS"));
	return "";
	
}

void UDataAnalyticsBPLibrary::ResetValues()
{
	FinalString.Empty();
}

FString UDataAnalyticsBPLibrary::GetFinalString()
{
	return FinalString;
}

void UDataAnalyticsBPLibrary::WriteToFile(FString FilePath, FString InString, bool& outSuccess)
{
	if(!FFileHelper::SaveStringToFile(InString,*FilePath))
	{
		outSuccess = false;
		return;
	}
	outSuccess = true;
}

void UDataAnalyticsBPLibrary::CreateOutput(FString& outString)
{
	FinalString = "";
	
	/*FinalString = HeaderNames;

	FinalString.Append(Values);*/
	outString = FinalString;
}


