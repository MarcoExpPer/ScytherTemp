// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxSideScrollCamera.h"

// Sets default values
ABoxSideScrollCamera::ABoxSideScrollCamera()
{

	PrimaryActorTick.bCanEverTick = true;


	main = CreateDefaultSubobject<UBoxComponent>( TEXT( "Main" ) );
	SetRootComponent( main );

	outsideBox = CreateDefaultSubobject<UBoxComponent>( TEXT( "Outside Box" ) );
	outsideBox->SetupAttachment( main );

	insideBox = CreateDefaultSubobject<UBoxComponent>( TEXT( "Inside Box" ) );
	insideBox->SetupAttachment( main );


	mainCamera = CreateDefaultSubobject<UCameraComponent>( TEXT( "Camera Component" ) );
	mainCamera->SetupAttachment( main );
}

// Called when the game starts or when spawned
void ABoxSideScrollCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoxSideScrollCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	player = UGameplayStatics::GetPlayerController( GetWorld(), 0 );
	player->SetViewTarget( this ); //Ponemos esta cámara como la activa del jugador

	if( !player )
	{
		//UE_LOG( LogTemp, Warning, TEXT( "No player?" ) );
		return;
	}

	viewportClient = GEngine->GameViewport;
	screenResolution = viewportClient->Viewport->GetSizeXY();
	screenWidth = screenResolution.X;
	screenHeight = screenResolution.Y;

	//*
	playerPosition = player->GetPawn()->GetActorLocation();
	float posZ = playerPosition.Z; // la altura. no deberia tocarse
	float posY = playerPosition.Y; // profundidad
	float posX = playerPosition.X; // movimiento lateral
	//*/
	
	FVector2D screenPlayerPosition;
	UGameplayStatics::ProjectWorldToScreen( player, playerPosition, screenPlayerPosition );

	float screenXofPlayer = screenPlayerPosition.X;
	float screenYofPlayer = screenPlayerPosition.Y;
	FVector fv = GetActorForwardVector();
	FVector vp = FVector( 0, 0, -1 );
	float a = fv.Y, b = fv.Z;
	// diam X es producto vectorial de fv por vp, (0, a, b) por (0, 0, -1), eso da (-a, 0, 0)
	// diam Z es producto vectorial de diamX por fv, (-a, 0, 0) por (0, a, b), eso da (0, ab, -a^2)
	FVector diamX( -a, 0, 0 ), diamZ( 0, a * b, -a * a );
	diamX.Normalize(); diamZ.Normalize();

	
	/*
	* Corners:
	* 5 ·--------------------------· 6
	*   |                          |
	*   |  1 .---------------. 2   |
	*   |    |               |     |
	*   |    |               |     |
	*   |    |               |     |
	*   |    |               |     |
	*   |  3 ·---------------· 4   |
	*   |                          |
	* 7 ·--------------------------· 8
	*/
	FVector2D Corner1, Corner2, Corner3, Corner4, Corner5, Corner6, Corner7, Corner8;


	FVector BoxExtent = insideBox->GetScaledBoxExtent();
	float boxWidth = BoxExtent.Y;
	float boxHeight = BoxExtent.Z;
	FTransform Transform = insideBox->GetComponentTransform();
	FVector center = Transform.GetLocation();

	FVector bCorner1 = center - diamX * boxWidth - diamZ * boxHeight;
	FVector bCorner2 = center + diamX * boxWidth - diamZ * boxHeight;
	FVector bCorner3 = center - diamX * boxWidth + diamZ * boxHeight;
	FVector bCorner4 = center + diamX * boxWidth + diamZ * boxHeight;


	UGameplayStatics::ProjectWorldToScreen( player, bCorner1, Corner1 );
	UGameplayStatics::ProjectWorldToScreen( player, bCorner2, Corner2 );
	UGameplayStatics::ProjectWorldToScreen( player, bCorner3, Corner3 );
	UGameplayStatics::ProjectWorldToScreen( player, bCorner4, Corner4 );



	BoxExtent = outsideBox->GetScaledBoxExtent();
	boxWidth = BoxExtent.Y;
	boxHeight = BoxExtent.Z;
	Transform = outsideBox->GetComponentTransform();
	center = Transform.GetLocation();

	FVector bCorner5 = center - diamX * boxWidth - diamZ * boxHeight;
	FVector bCorner6 = center + diamX * boxWidth - diamZ * boxHeight;
	FVector bCorner7 = center - diamX * boxWidth + diamZ * boxHeight;
	FVector bCorner8 = center + diamX * boxWidth + diamZ * boxHeight;

	UGameplayStatics::ProjectWorldToScreen( player, bCorner5, Corner5 );
	UGameplayStatics::ProjectWorldToScreen( player, bCorner6, Corner6 );
	UGameplayStatics::ProjectWorldToScreen( player, bCorner7, Corner7 );
	UGameplayStatics::ProjectWorldToScreen( player, bCorner8, Corner8 );

	if( debugPositions )
	{
		UE_LOG( LogTemp, Warning, TEXT( "-------------------------------------------------" ) );
		UE_LOG( LogTemp, Warning, TEXT( "  sc: %f, %f" ), screenWidth, screenHeight );
		UE_LOG( LogTemp, Warning, TEXT( "  Player: %f, %f" ), screenXofPlayer, screenYofPlayer );
		UE_LOG( LogTemp, Warning, TEXT( "      fv: %f, %f, %f" ), fv.X, fv.Y, fv.Z );
		UE_LOG( LogTemp, Warning, TEXT( "Numero %d: %f, %f" ), 1, Corner1.X, Corner1.Y );
		UE_LOG( LogTemp, Warning, TEXT( "Numero %d: %f, %f" ), 2, Corner2.X, Corner2.Y );
		UE_LOG( LogTemp, Warning, TEXT( "Numero %d: %f, %f" ), 3, Corner3.X, Corner3.Y );
		UE_LOG( LogTemp, Warning, TEXT( "Numero %d: %f, %f" ), 4, Corner4.X, Corner4.Y );
		UE_LOG( LogTemp, Warning, TEXT( "Numero %d: %f, %f" ), 5, Corner5.X, Corner5.Y );
		UE_LOG( LogTemp, Warning, TEXT( "Numero %d: %f, %f" ), 6, Corner6.X, Corner6.Y );
		UE_LOG( LogTemp, Warning, TEXT( "Numero %d: %f, %f" ), 7, Corner7.X, Corner7.Y );
		UE_LOG( LogTemp, Warning, TEXT( "Numero %d: %f, %f" ), 8, Corner8.X, Corner8.Y );
		UE_LOG( LogTemp, Warning, TEXT( "-------------------------------------------------" ) );
	}

	float offset, offsetExtra;
	if( screenXofPlayer > Corner6.X )
	{
		offset = screenXofPlayer - Corner6.X;
		offsetExtra = screenXofPlayer - Corner2.X;
		SetActorLocation( GetActorLocation() + FVector( offset + offsetExtra / 1000, 0, 0 ) ); 
			// sin el offsetExtra se queda igualado en la caja exterior
	}
	else if( screenXofPlayer > Corner2.X && screenXofPlayer <= Corner6.X )
	{
		offset = screenXofPlayer - Corner2.X;
		SetActorLocation( GetActorLocation() + FVector( offset / 25, 0, 0 ) );
	}
	else if( screenXofPlayer < Corner5.X )
	{
		offset = screenXofPlayer - Corner5.X;
		offsetExtra = screenXofPlayer - Corner1.X;
		SetActorLocation( GetActorLocation() + FVector( offset + offsetExtra / 1000, 0, 0 ) );
	}
	else if( screenXofPlayer < Corner1.X && screenXofPlayer >= Corner5.X )
	{
		offset = screenXofPlayer - Corner1.X;
		SetActorLocation( GetActorLocation() + FVector( offset / 25, 0, 0 ) );
	}
	if( screenYofPlayer > Corner7.Y )
	{
		offset = screenYofPlayer - Corner7.Y;
		offsetExtra = screenYofPlayer - Corner3.Y;
		SetActorLocation( GetActorLocation() + FVector( 0, offset + offsetExtra / 1000, 0 ) );
	}
	else if( screenYofPlayer > Corner3.Y && screenYofPlayer <= Corner7.Y )
	{
		offset = screenYofPlayer - Corner3.Y;
		SetActorLocation( GetActorLocation() + FVector( 0, offset / 25, 0 ) );
	}
	else if( screenYofPlayer < Corner5.Y )
	{
		offset = screenYofPlayer - Corner5.Y;
		offsetExtra = screenYofPlayer - Corner1.Y;
		SetActorLocation( GetActorLocation() + FVector( 0, offset + offsetExtra / 1000, 0 ) );
	}
	else if( screenYofPlayer < Corner1.Y && screenYofPlayer >= Corner5.Y )
	{
		offset = screenYofPlayer - Corner1.Y;
		SetActorLocation( GetActorLocation() + FVector( 0, offset / 10, 0 ) );
	}


}

