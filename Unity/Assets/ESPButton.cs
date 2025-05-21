using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

[RequireComponent(typeof(Button))]
public class ESPButton : MonoBehaviour
{
    [Tooltip("The message to send over HTTP.")]
    public string command;

    [Tooltip("Optional: the scene to load after sending.")]
    public string sceneToLoad;

    void Awake()
    {
        GetComponent<Button>().onClick.AddListener(OnPress);
    }

    void OnPress()
    {
        // Always hits the one true instance
        if (NetworkManager.Instance?.client != null)
            NetworkManager.Instance.client.SendCommand(command);
        else
            Debug.LogError("[ESPButton] No NetworkManager.Instance.client!");

        if (!string.IsNullOrEmpty(sceneToLoad))
            SceneManager.LoadScene(sceneToLoad);
    }
}
