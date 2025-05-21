using UnityEngine;
using UnityEngine.SceneManagement;

public class SceneChangeScript : MonoBehaviour
{
    
    public void goToMenu()
    {
        SceneManager.LoadScene("GalacticMenuFix");
    }

    public void goToEarth()
    {
        SceneManager.LoadScene("EarthEnvironment");
    }

    public void goToMars()
    {
        SceneManager.LoadScene("MarsEnvironment");
    }


    public void goToJupiter()
    {
        SceneManager.LoadScene("JupiterEnvironment");
    }

    public void goToExperiment()
    {
        SceneManager.LoadScene("ExperimentEnvironment");
    }
}
