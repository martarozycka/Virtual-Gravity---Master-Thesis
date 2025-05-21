using UnityEngine;
using UnityEngine.Networking;
using System.Collections;
public class ESPHttpClient : MonoBehaviour
{
    [SerializeField] // Set the IP address of your ESP8266 – this is the one printed by the ESP (e.g., 172.20.10.8)

    private string espIPAddress = "172.20.10.8";  // Update if needed

    [SerializeField]
    private int port = 80; // The port on which the ESP8266 HTTP server is listening (default is 80)

    void Awake()
        {
            Debug.Log($"[ESPHttpClient] activeSelf={gameObject.activeSelf}, activeInHierarchy={gameObject.activeInHierarchy}");
            DontDestroyOnLoad(gameObject);
        }

    public void SendCommand(string command) // Public method to send a command to the ESP8266 via an HTTP GET request.
    {
        StartCoroutine(SendCommandCoroutine(command));
    }
    private IEnumerator SendCommandCoroutine(string command)
    {
        string url = $"http://{espIPAddress}:{port}/command?msg={command}";
        Debug.Log($"[ESPHttpClient] Sending GET request: {url}");

        UnityWebRequest request = UnityWebRequest.Get(url);
        yield return request.SendWebRequest();
        // Check for errors
        if (request.result != UnityWebRequest.Result.Success)
        {
            Debug.LogError($"[ESPHttpClient] HTTP request error: {request.error}");
        }
        else
        {
            Debug.Log($"[ESPHttpClient] Response from ESP8266: {request.downloadHandler.text}");
        }
    }
}
