using UnityEngine;

public class Rotator : MonoBehaviour
{
    public float rotationSpeed = 100f; // Adjust this value to control the rotation speed

    void Update()
    {
        // Rotate the object around the Y-axis
        transform.Rotate(0, rotationSpeed * Time.deltaTime, 0);
    }
}
