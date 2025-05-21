using UnityEngine;

public class NetworkManager : MonoBehaviour
{
    // Singleton instance
    public static NetworkManager Instance { get; private set; }

    [Tooltip("Drag your ESPHttpClient component here")]
    public ESPHttpClient client;

    void Awake()
    {
        // If another instance already exists, destroy this one
        if (Instance != null && Instance != this)
        {
            Destroy(gameObject);
            return;
        }

        Instance = this;
        DontDestroyOnLoad(gameObject);

        // Just in case someone deactivated the GameObject in the editor
        if (!gameObject.activeSelf)
            gameObject.SetActive(true);
    }
}



