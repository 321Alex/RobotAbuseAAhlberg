#include "Misc/AutomationTest.h"
#include "AttachablePart.h"
#include "AttachmentPoint.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FAttachmentCompatibilityTest_Final,
    "RobotAbuse.AttachmentSystem.CompatibilityCheck",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FAttachmentCompatibilityTest_Final::RunTest(const FString& Parameters)
{ 
    UAttachmentPoint* LeftSocket = NewObject<UAttachmentPoint>();
    LeftSocket->AcceptedArmType = EArmType::Left;
    
    AAttachablePart* LeftArm = NewObject<AAttachablePart>();
    LeftArm->ArmType = EArmType::Left;
    
    AAttachablePart* RightArm = NewObject<AAttachablePart>();
    RightArm->ArmType = EArmType::Right;

    TestTrue(TEXT("Left arm should be accepted by left socket"), 
             LeftSocket->CanAcceptPart(LeftArm));

    TestFalse(TEXT("Right arm should be rejected by left socket"), 
              LeftSocket->CanAcceptPart(RightArm));

    TestFalse(TEXT("Null part should be rejected"), 
              LeftSocket->CanAcceptPart(nullptr));
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FPartStateTransitionTest_Final,
    "RobotAbuse.AttachablePart.StateTransitions",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FPartStateTransitionTest_Final::RunTest(const FString& Parameters)
{
    AAttachablePart* Part = NewObject<AAttachablePart>();
    
    // Test initial state
    TestEqual(TEXT("Part should start in DETACHED state"), 
              Part->CurrentState, EPartState::DETACHED);
    
    // Test PickUp changes state
    Part->PickUp();
    TestEqual(TEXT("Part should be HELD after PickUp"), 
              Part->CurrentState, EPartState::HELD);
    
    // Test Drop changes state back
    Part->Drop();
    TestEqual(TEXT("Part should be DETACHED after Drop"), 
              Part->CurrentState, EPartState::DETACHED);
    
    return true;
}
