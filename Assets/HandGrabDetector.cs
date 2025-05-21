using UnityEngine;

public class HandGrabDetector : MonoBehaviour
{
    public OVRHand leftHand;
    public OVRHand rightHand;

    public ESPHttpClient sender; // Reference to the HTTPSender script
    private bool wasGrabbing = false;

    void Awake()
    {
        if (NetworkManager.Instance != null)
            sender = NetworkManager.Instance.client;
        else
            Debug.LogError("NetworkManager.Instance not found!");
    }


    void Update()
    {
        bool isLeftGrabbing = leftHand.GetFingerIsPinching(OVRHand.HandFinger.Index) && leftHand.GetFingerIsPinching(OVRHand.HandFinger.Thumb);
        bool isRightGrabbing = rightHand.GetFingerIsPinching(OVRHand.HandFinger.Index) && rightHand.GetFingerIsPinching(OVRHand.HandFinger.Thumb);

        bool isGrabbingNow = isLeftGrabbing || isRightGrabbing;

        if (isGrabbingNow && !wasGrabbing)
        {
            Debug.Log("Grab started!");
            sender.SendCommand("User Grabbed!");
        }

        wasGrabbing = isGrabbingNow;
    }
}
