using System;
using DiamondEngine;

public class Skill_Tree_Node : DiamondComponent
{

    public enum NODE_STATE
    {
        UNLOCKED,
        LOCKED,
        OWNED,
    };

    #region Style
    public int unlockedButtonPressed = 0;
    public int unlockedButtonHovered = 0;
    public int unlockedButtonUnhovered = 0;

    public int lockedButtonHovered = 0;
    public int lockedButtonUnhovered = 0;

    public int ownedButtonPressed = 0;
    public int ownedButtonHovered = 0;
    public int ownedButtonUnhovered = 0;
    #endregion

    #region Logic
    public string skill_name = "";

    public GameObject children_1 = null;
    public GameObject children_2 = null;
    public GameObject parent_1 = null;
    public GameObject parent_2 = null;
    public GameObject oppositeNode = null;

    public GameObject text_description = null;
    public GameObject hub_skill_controller = null; 

    private Skills skill = null;

    public bool isRootNode = false;
    private NODE_STATE _state;

    public int node_type = 0;

    public int skillTreeName;
    public int skillTreeNumber;

    private bool started = false;

    public NODE_STATE state 
    {
        get { return _state; }
        set 
        {
            _state = value;
            switch (value)
            {
                case NODE_STATE.UNLOCKED:
                    gameObject.GetComponent<Button>().ChangeSprites(unlockedButtonPressed, unlockedButtonHovered, unlockedButtonUnhovered);
                    break;
                case NODE_STATE.LOCKED:
                    gameObject.GetComponent<Button>().ChangeSprites(lockedButtonHovered, lockedButtonHovered, lockedButtonUnhovered);
                    break;
                case NODE_STATE.OWNED:
                    gameObject.GetComponent<Button>().ChangeSprites(ownedButtonPressed, ownedButtonHovered, ownedButtonUnhovered);
                    break;
                default:
                    break;
            }
        }
    }
    #endregion

    public void Awake()
    {
        UnlockTreeAfterRun();
    }

    //Remember the skills that have already been bought before the run
    private void UnlockTreeAfterRun()
    {
        if (Skill_Tree_Data.IsEnabled(skillTreeName, skillTreeNumber)) //Edge case: The node is already OWNED
        {
            state = NODE_STATE.OWNED;
        }
        else if (oppositeNode != null) //Edge case: Nodes with opposite
        {
            if (oppositeNode.GetComponent<Skill_Tree_Node>().skillTreeNumber > skillTreeNumber) //oppositeNode = right node
            {
                if (Skill_Tree_Data.IsEnabled(skillTreeName, skillTreeNumber + 1))
                    state = NODE_STATE.LOCKED;
                else if (isRootNode)
                    state = NODE_STATE.UNLOCKED;
                else if (Skill_Tree_Data.IsEnabled(skillTreeName, skillTreeNumber - 1)) //Check if parent is enabled
                    state = NODE_STATE.UNLOCKED;
                else
                    state = NODE_STATE.LOCKED;
            }
            else if (oppositeNode.GetComponent<Skill_Tree_Node>().skillTreeNumber < skillTreeNumber) //oppositeNode = left node
            {
                if (Skill_Tree_Data.IsEnabled(skillTreeName, skillTreeNumber + -1))
                    state = NODE_STATE.LOCKED;
                else if (isRootNode) 
                    state = NODE_STATE.UNLOCKED;
                else if (Skill_Tree_Data.IsEnabled(skillTreeName, skillTreeNumber - 2)) //Check if parent is enabled
                    state = NODE_STATE.UNLOCKED;
                else
                    state = NODE_STATE.LOCKED;
            }
            else
            {
                state = NODE_STATE.LOCKED;
            }
        }
        else //Edge case: Nodes without opposite
        {
            if (isRootNode && Skill_Tree_Data.IsEnabled(skillTreeName, skillTreeNumber) == false)
                state = NODE_STATE.UNLOCKED;
            else if(parent_2 == null) //Only has one parent
            {
                if (Skill_Tree_Data.IsEnabled(skillTreeName, skillTreeNumber - 1)) //Parent is already OWNED
                    state = NODE_STATE.UNLOCKED;
                else
                    state = NODE_STATE.LOCKED;
            }
            else if(parent_1 != null && parent_2 != null) //It has two parents
            {
                if (Skill_Tree_Data.IsEnabled(skillTreeName, skillTreeNumber - 1) || Skill_Tree_Data.IsEnabled(skillTreeName, skillTreeNumber - 2))
                    state = NODE_STATE.UNLOCKED;
                else
                    state = NODE_STATE.LOCKED;
            }
            else //Both parents are NULL
            {
                state = NODE_STATE.LOCKED; 
            }
        }
    }

    public void Update()
	{
        if (started == false && Skill_Tree_Data.data_entry_assigned)
        {
            AssignCharacteristics();
            started = true;
        }

        if (skill == null)
            return;

        if (gameObject.GetComponent<Navigation>().is_selected == false)
            return;

        if (hub_skill_controller == null)
            return;

        if (hub_skill_controller.GetComponent<HubSkillTreeController>().skill_selected == skill)
            return;

        hub_skill_controller.GetComponent<HubSkillTreeController>().skill_selected = skill;

        if (text_description != null)
            text_description.GetComponent<Text>().text = skill.description;

        if (Input.GetGamepadButton(DEControllerButton.Y) == KeyState.KEY_DOWN)
        {
            Debug.Log("Beskar: ");
            PlayerResources.AddResourceBy1(RewardType.REWARD_BESKAR);
            Debug.Log("Beskar: " + PlayerResources.GetResourceCount(RewardType.REWARD_BESKAR));
        }
    }

    private void AssignCharacteristics()
    {
        if (SkillDictionary.skill_type.ContainsKey(skill_name))
        {
            Type t = SkillDictionary.skill_type[skill_name];
            skill = (Skills)Activator.CreateInstance(t);
            skill.AssignCharacteristics();
        }
        else
        {
            Debug.Log("ERROR: Skill doesn't exist");
        }

        if (node_type == 0)
            skill.type_of_price = RewardType.REWARD_BESKAR;
        else if (node_type == 1)
            skill.type_of_price = RewardType.REWARD_MACARON;
        else
            skill.type_of_price = RewardType.REWARD_SCRAP;        
    }

    public void OnExecuteButton()
    {
        if (state == NODE_STATE.LOCKED || state == NODE_STATE.OWNED)
            return;
       
        if (skill.type_of_price == RewardType.REWARD_BESKAR)
        {
            if (PlayerResources.GetResourceCount(RewardType.REWARD_BESKAR) < skill.price)
            {
                Debug.Log("You don't have enough Beskar!");
                return;
            }
            else
            {
                PlayerResources.SubstractResource(RewardType.REWARD_BESKAR, skill.price);
                Debug.Log("Beskar: " + PlayerResources.GetResourceCount(RewardType.REWARD_BESKAR));
            }
        }

        if (skill.type_of_price == RewardType.REWARD_MACARON)
        {
            if (PlayerResources.GetResourceCount(RewardType.REWARD_MACARON) < skill.price)
            {
                Debug.Log("You don't have enough Macarons!");
                return;
            }
            else
            {
                PlayerResources.SubstractResource(RewardType.REWARD_MACARON, skill.price);
                Debug.Log("Macarons: " + PlayerResources.GetResourceCount(RewardType.REWARD_MACARON));
            }
        }

        if (skill.type_of_price == RewardType.REWARD_SCRAP)
        {
            if (PlayerResources.GetResourceCount(RewardType.REWARD_SCRAP) < skill.price)
            {
                Debug.Log("You don't have enough Scrap!");
                return;
            }
            else
            {
                PlayerResources.SubstractResource(RewardType.REWARD_SCRAP, skill.price);
                Debug.Log("Scrap: " + PlayerResources.GetResourceCount(RewardType.REWARD_SCRAP));
            }
        }
        skill.Use();
        adSkill(skill_name);
        state = NODE_STATE.OWNED;

        if (children_1 != null)
            children_1.GetComponent<Skill_Tree_Node>().state = NODE_STATE.UNLOCKED;

        if (children_2 != null)
            children_2.GetComponent<Skill_Tree_Node>().state = NODE_STATE.UNLOCKED;

        if(oppositeNode != null)
            oppositeNode.GetComponent<Skill_Tree_Node>().state = NODE_STATE.LOCKED;
    }

    private void adSkill(string name)
    {
        switch(name)
        {
            case "UKnockback":
                Debug.Log(name);
                break;
        }
    }
}