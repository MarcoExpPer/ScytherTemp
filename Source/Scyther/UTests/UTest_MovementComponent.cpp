// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreTypes.h"
#include "Misc/AutomationTest.h"
#include <Scyther/Components/3D_MovementComponent.h>

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST( FHitAngleTest, "Project.UnitTests.HitAngleTest", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter )


bool FHitAngleTest::RunTest( const FString& Parameters )
{
	//HitAngleDegree must return the correct angle
	{
		FHitResult hit1 = FHitResult();
		hit1.Normal = FVector( 1, 0, 0 );

		FHitResult hit2 = FHitResult();
		hit2.Normal = FVector( 0, 1, 0 );

		FHitResult hit3 = FHitResult();
		hit3.Normal = FVector( 0, 0, 1 );

		FHitResult hit4 = FHitResult();
		hit4.Normal = FVector( 1, 0, 1 );

		FHitResult hit5 = FHitResult();
		hit5.Normal = FVector( 0, 1, 1 );

		FHitResult hit6 = FHitResult();
		hit6.Normal = FVector( 1, 1, 1 );

		TestEqual( TEXT( "HitAngleDegree(1, 0, 0) must return the correct angle with the floor" ), U3D_MovementComponent::GetHitDegreeAngle( hit1 ), FMath::Abs( 90.f - 180.f ) );
		TestEqual( TEXT( "HitAngleDegree(0, 1, 0) must return the correct angle with the floor" ), U3D_MovementComponent::GetHitDegreeAngle( hit2 ), FMath::Abs( 90.f - 180.f) );
		TestEqual( TEXT( "HitAngleDegree(0, 0, 1) must return the correct angle with the floor" ), U3D_MovementComponent::GetHitDegreeAngle( hit3 ), FMath::Abs( 180.f - 180.f) );
		TestEqual( TEXT( "HitAngleDegree(1, 0, 1) must return the correct angle with the floor" ), U3D_MovementComponent::GetHitDegreeAngle( hit4 ), FMath::Abs( 135.f - 180.f) );
		TestEqual( TEXT( "HitAngleDegree(0, 1, 1) must return the correct angle with the floor" ), U3D_MovementComponent::GetHitDegreeAngle( hit5 ), FMath::Abs( 135.f - 180.f) );
		TestEqual( TEXT( "HitAngleDegree(1, 1, 1) must return the correct angle with the floor" ), U3D_MovementComponent::GetHitDegreeAngle( hit6 ), FMath::RoundToFloat( FMath::Abs(125.26 - 180.f)) );
	}

	return true;
}


#endif //WITH_DEV_AUTOMATION_TESTS
