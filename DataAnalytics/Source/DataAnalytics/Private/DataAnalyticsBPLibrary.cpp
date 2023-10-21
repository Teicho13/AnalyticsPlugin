// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataAnalyticsBPLibrary.h"
#include "DataAnalytics.h"
#include "Misc/FileHelper.h"

DEFINE_LOG_CATEGORY(AnalyticsLog);

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
			UE_LOG(AnalyticsLog, Warning, TEXT("Header: %s"), *PropertyIt->GetAuthoredName());

			FinalString.Append(*PropertyIt->GetAuthoredName());
			FinalString.Append("\n");
			
			//Check value dimensions
			for (int i = 0; i < PropertyIt->ArrayDim; ++i)
			{
				//Get pointer to item in array
				void* ValuePtr = PropertyIt->ContainerPtrToValuePtr<void>(structPtr,i);

				//Parse item
				FString temp = ParseStructData(*PropertyIt, ValuePtr, layer);

				FinalString.Append(temp);
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
			UE_LOG(AnalyticsLog, Warning, TEXT("Parse: Is an Int"));
			return TempString;
		}
		//Is a float
		if(NumericProperty->IsFloatingPoint())
		{
			FloatValue = NumericProperty->GetFloatingPointPropertyValue(valuePtr);
			FString FloatStr = FString::SanitizeFloat(FloatValue);
			TempString.Append(FloatStr);
			UE_LOG(AnalyticsLog, Warning, TEXT("Parse: Is a Float"));
			return TempString;
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
			UE_LOG(AnalyticsLog, Warning, TEXT("Parse: Is a Bool (True)"));
			return TempString;
		}else
		{
			//false
			TempString.Append("False");
			UE_LOG(AnalyticsLog, Warning, TEXT("Parse: Is a Bool (False)"));
			return TempString;
		}
	}
	//Check for names
	else if (FNameProperty* NameProperty = CastField<FNameProperty>(prop))
	{
		NameValue = NameProperty->GetPropertyValue(valuePtr);
		TempString.Append(NameValue.ToString());
		UE_LOG(AnalyticsLog, Warning, TEXT("Parse: Is a Name"));
		return TempString;
	}
	//Check for string
	else if (FStrProperty* StringProperty = CastField<FStrProperty>(prop))
	{
		StringValue = StringProperty->GetPropertyValue(valuePtr);
		TempString.Append(StringValue);
		UE_LOG(AnalyticsLog, Warning, TEXT("Parse: Is a String"));
		return TempString;
	}
	//Check for text
	else if (FTextProperty* TextProperty = CastField<FTextProperty>(prop))
	{
		TextValue = TextProperty->GetPropertyValue(valuePtr);
		TempString.Append(TextValue.ToString());
		UE_LOG(AnalyticsLog, Warning, TEXT("Parse: Is a Text"));
		return TempString;
	}
	//Check for array
	else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(prop))
	{
		UE_LOG(AnalyticsLog, Warning, TEXT("Parse: Is an Array"));

		
		//Get a helper to go trough array
		FScriptArrayHelper Helper(ArrayProperty,valuePtr);

		
		//Check if array item is an struct
		FStructProperty* StructProperty = CastField<FStructProperty>(ArrayProperty->Inner);
		if(StructProperty)
		{
			//Loop trough and parse all array values
			for (int i = 0, n = Helper.Num(); i < n; ++i)
			{
				ParseStructData(ArrayProperty->Inner,Helper.GetRawPtr(i),layer + 1);
			}
		}else
		{
			//Open quote to group values
			TempString.Append("\u0022");

			//Loop trough and parse all array values
			for (int i = 0, n = Helper.Num(); i < n; ++i)
			{
				TempString.Append(ParseStructData(ArrayProperty->Inner,Helper.GetRawPtr(i),layer + 1));

				//If there are more than 1 values Add a seperator
				if(i < n - 1)
				{
					TempString.Append(" | ");
				}
			}
			//Close quotes
			TempString.Append("\u0022");
		}
		
		return TempString;
	}
	// Reading a nested struct
	else if (prop)
	{
		UE_LOG(AnalyticsLog, Warning, TEXT("Parse: Is a Struct"));

		//Go into struct again
		FStructProperty* StructProperty = CastField<FStructProperty>(prop);
		if(StructProperty)
		{
			for (TFieldIterator<FProperty> PropertyIt(StructProperty->Struct); PropertyIt; ++PropertyIt)
			{
				UE_LOG(AnalyticsLog, Warning, TEXT("Sub Header: %s"), *PropertyIt->GetAuthoredName());
				HeaderString.Append(*PropertyIt->GetAuthoredName());
				HeaderString.Append(",");
				
				//Check value dimensions
				for (int i = 0; i < PropertyIt->ArrayDim; ++i)
				{
					void* ValuePtr = PropertyIt->ContainerPtrToValuePtr<void>(valuePtr,i);

					FString temp = ParseStructData(*PropertyIt, ValuePtr, layer);
					ValueString.Append(temp);
					ValueString.Append(",");
					
				}
			}

			//Remove last header comma
			int cutSize = HeaderString.Len() - 1;
			HeaderString.RemoveAt(cutSize);
			HeaderString.Append("\n");

			//Remove last Value comma
			cutSize = ValueString.Len() - 1;
			ValueString.RemoveAt(cutSize);
			ValueString.Append("\n");

			//Add both rows to output
			FinalString.Append(HeaderString);
			FinalString.Append(ValueString);

			//Clear values
			HeaderString.Empty();
			ValueString.Empty();
			
			return TempString;
		}
		
		return "";
	}

	UE_LOG(AnalyticsLog, Warning, TEXT("PARSE: NO RESULTS"));
	return "";
	
}

void UDataAnalyticsBPLibrary::ResetValues()
{
	FinalString.Empty();
	HeaderString.Empty();
	ValueString.Empty();
}

FString UDataAnalyticsBPLibrary::GetAnalyticsOutput()
{
	return FinalString;
}

void UDataAnalyticsBPLibrary::WriteToCSV(FString FilePath, FString InString, bool& outSuccess)
{
	//remove Whitespace
	FilePath = FilePath.TrimStartAndEnd();
	
	//Check for CSV file extension
	const FString stringToCheck = TEXT(".csv");

	if(FilePath.EndsWith(stringToCheck))
	{
		if(!FFileHelper::SaveStringToFile(InString,*FilePath))
		{
			outSuccess = false;
			return;
		}
	}else
	{
		outSuccess = false;
		UE_LOG(AnalyticsLog, Error, TEXT("Could not write to CSV, FilePath incorrect (Incorrect extension)"));
		return;
	} 
	outSuccess = true;
}

