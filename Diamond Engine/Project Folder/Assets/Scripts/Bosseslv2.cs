using System;
using System.Collections.Generic;
using DiamondEngine;

public class Bosseslv2 : DiamondComponent
{
    public enum BOSS_STATE : int
    {
        NONE = -1,
        SEARCH_STATE,
        FOLLOW,
        WANDER,
        FAST_RUSH,
        SLOW_RUSH,
        RUSH_STUN,
        PROJECTILE,
        JUMP_SLAM,
        BOUNCE_RUSH,
        DEAD
    }

    public enum BOSS_INPUT : int
    {
        NONE = -1,
        IN_FOLLOW,
        IN_FOLLOW_END,
        IN_WANDER,
        IN_WANDER_END,
        IN_FAST_RUSH,
        IN_FAST_RUSH_END,
        IN_SLOW_RUSH,
        IN_SLOW_RUSH_END,
        IN_RUSH_STUN,
        IN_RUSH_STUN_END,
        IN_PROJECTILE,
        IN_PROJECTILE_END,
        IN_JUMPSLAM,
        IN_JUMPSLAM_END,
        IN_BOUNCERUSH,
        IN_BOUNCERUSH_END,
        IN_DEAD
    }

    public NavMeshAgent agent = null;
    public Random randomNum = new Random();

    public BOSS_STATE currentState = BOSS_STATE.SEARCH_STATE;
    public List<BOSS_INPUT> inputsList = new List<BOSS_INPUT>();

    public float slerpSpeed = 5.0f;

    //Public Variables
    public float probFollow = 60.0f;
    public float probWander = 40.0f;
    public GameObject projectilePoint = null;
    public float distanceProjectile = 15.0f;
    public float wanderRange = 7.5f;


    //Private Variables
    public bool resting = false;
    public bool firstShot = false;

    //Stats
    public float healthPoints = 1920.0f;
    public float speed = 3.5f;
    public float fastRushSpeed = 14.0f;
    public float slowRushSpeed = 7.0f;

    //Timers
    public float walkingTime = 4.0f;
    public float walkingTimer = 0.0f;
    public float fastChasingTime = 0.5f;
    public float fastChasingTimer = 0.0f;
    public float slowChasingTime = 3.5f;
    public float slowChasingTimer = 0.0f;
    public float shootingTime = 1.0f;
    public float shootingTimer = 0.0f;
    public float dieTime = 2.0f;
    public float dieTimer = 0.0f;
    public float restingTime = 4.0f;
    public float restingTimer = 0.0f;

    //Atacks
    public float projectileAngle = 30.0f;
    public float projectileRange = 6.0f;
    public float projectileDamage = 10.0f;
    public float rushDamage = 15.0f;

    #region PROJECTILE
    public void StartProjectile()
    {
        Debug.Log("Starting Throwing Projectile");
        shootingTimer = shootingTime;
        firstShot = true;
    }
    public void UpdateProjectile()
    {
        if (shootingTimer > 0.0f)
        {
            shootingTimer -= Time.deltaTime;
            if (shootingTimer < 0.0f)
            {
                if (projectilePoint != null)
                {
                    Vector3 pos = projectilePoint.transform.globalPosition;
                    Quaternion rot = projectilePoint.transform.globalRotation;
                    Vector3 scale = new Vector3(1, 1, 1);

                    GameObject projectile = InternalCalls.CreatePrefab("Library/Prefabs/tocreate.prefab", pos, rot, scale);
                    projectile.GetComponent<RancorProjectile>().targetPos = Core.instance.gameObject.transform.globalPosition;
                    if (firstShot)
                    {
                        shootingTimer = shootingTime;
                        firstShot = false;
                    }
                }
            }
        }
        Debug.Log("Projectile");
        Debug.Log(shootingTimer.ToString());
        Debug.Log("Prohjectile null");

    }

    public void EndProjectile()
    {

    }
    #endregion

    #region RUSH
    public void StartFastRush()
    {
        fastChasingTimer = fastChasingTime;
        Debug.Log("Fast Rush");
    }
    public void UpdateFastRush()
    {
        agent.CalculatePath(gameObject.transform.globalPosition, Core.instance.gameObject.transform.globalPosition);
        LookAt(agent.GetDestination());
        agent.MoveToCalculatedPos(fastRushSpeed);
        Debug.Log("Rush");
    }

    public void EndFastRush()
    {

    }

    public void StartSlowRush()
    {
        slowChasingTimer = slowChasingTime;
        Debug.Log("Slow Rush");
    }
    public void UpdateSlowRush()
    {
        agent.CalculatePath(gameObject.transform.globalPosition, Core.instance.gameObject.transform.globalPosition);
        LookAt(agent.GetDestination());
        agent.MoveToCalculatedPos(slowRushSpeed);
    }

    public void EndSlowRush()
    {

    }
    #endregion

    #region RUSH STUN
    public void StartRushStun()
    {

    }
    public void UpdateRushStun()
    {
        Debug.Log("Rush Stun");
    }

    public void EndRushStun()
    {

    }
    #endregion

    #region FOLLOW
    public void StartFollowing()
    {
        walkingTimer = walkingTime;
    }
    public void UpdateFollowing()
    {
        agent.CalculatePath(gameObject.transform.globalPosition, Core.instance.gameObject.transform.globalPosition);
        LookAt(agent.GetDestination());
        agent.MoveToCalculatedPos(speed);
        Debug.Log("Following player");
    }

    public void EndFollowing()
    {

    }
    #endregion

    #region WANDER
    public void StartWander()
    {
        walkingTimer = walkingTime;
        agent.CalculateRandomPath(gameObject.transform.globalPosition, wanderRange);
        Debug.Log("Wander");
    }
    public void UpdateWander()
    {
        LookAt(agent.GetDestination());
        agent.MoveToCalculatedPos(speed);
        Debug.Log("Following player");
    }

    public void EndWander()
    {

    }
    #endregion

    #region DIE
    public void StartDie()
    {
        dieTimer = dieTime;
        Debug.Log("Dying");
    }
    public void UpdateDie()
    {
        if (dieTimer > 0.0f)
        {
            dieTimer -= Time.deltaTime;

            if (dieTimer <= 0.0f)
            {
                EndDie();
            }
        }
    }

    public void EndDie()
    {
        Debug.Log("WAMPA DEAD");

        Counter.roomEnemies--;

        if (Counter.roomEnemies <= 0)
            Counter.allEnemiesDead = true;

        EnemyManager.RemoveEnemy(gameObject);
        InternalCalls.Destroy(gameObject);
    }
    #endregion

    public void LookAt(Vector3 pointToLook)
    {
        Vector3 direction = pointToLook - gameObject.transform.globalPosition;
        direction = direction.normalized;
        float angle = (float)Math.Atan2(direction.x, direction.z);

        if (Math.Abs(angle * Mathf.Rad2Deg) < 1.0f)
            return;

        Quaternion dir = Quaternion.RotateAroundAxis(Vector3.up, angle);

        float rotationSpeed = Time.deltaTime * slerpSpeed;

        Quaternion desiredRotation = Quaternion.Slerp(gameObject.transform.localRotation, dir, rotationSpeed);

        gameObject.transform.localRotation = desiredRotation;
    }
}