using UnityEngine;
using Oculus.Interaction;               // for InteractorState
using Oculus.Interaction.HandGrab;  // for HandGrabInteractable
using System.Linq;                      // for Any()
// (no extra assembly definitions needed if you’ve already imported the HandGrab sample)

[RequireComponent(typeof(HandGrabInteractable))]
public class GrabEventSender : MonoBehaviour
{
    HandGrabInteractable _interactable;
    ESPHttpClient        _client;
    bool                 _wasGrabbed;

    void Awake()
    {
        _interactable = GetComponent<HandGrabInteractable>();
        _client       = NetworkManager.Instance.client; 
        _wasGrabbed   = false;
    }

    void Update()
    {
        // if any hand-interactor is currently grabbing this interactable...
        bool isGrabbed = _interactable.Interactors
                              .Any(i => i.State == InteractorState.Select);

        if (isGrabbed && !_wasGrabbed)
        {
            // just started grabbing
            Debug.Log(">> OnGrab()");
            _client.SendCommand("objectHeld");
        }
        else if (!isGrabbed && _wasGrabbed)
        {
            // just released
            Debug.Log(">> OnRelease()");
            _client.SendCommand("systemShutdown");
        }

        _wasGrabbed = isGrabbed;
    }
}
