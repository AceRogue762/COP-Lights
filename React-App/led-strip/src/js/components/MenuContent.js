/**
 * Main app component displaying menu and selected content
 */

import React, { Component } from "react";
import AppBar from '@material-ui/core/AppBar';
import Toolbar from '@material-ui/core/Toolbar';
import Typography from '@material-ui/core/Typography';
import IconButton from '@material-ui/core/IconButton';
import PowerSettingsNewIcon from '@material-ui/icons/PowerSettingsNew';
import PaletteIcon from '@material-ui/icons/Palette';
import WbIncandescentIcon from '@material-ui/icons/WbIncandescent';
import CodeIcon from '@material-ui/icons/Code';
import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemText from '@material-ui/core/ListItemText';
import Divider from '@material-ui/core/Divider';
import Drawer from '@material-ui/core/Drawer';
import Hidden from '@material-ui/core/Hidden';
import { withStyles } from "@material-ui/core/styles";
import MenuIcon from '@material-ui/icons/Menu';
import WhiteTextTypography from './WhiteTextTypography';

// Import components
import AnimationButtonList from "./AnimationButtonList";
import EffectPicker from "./EffectPicker";
import DevloperPage from "./DeveloperPage";

/**
 * Map menu button id to:
 * title: Text displayed in the menu selection item
 * icon: Icon displayed next to the title
 * content: Component that should be rendered upon selecting the button
 */
const contentMap = {
  'animations': {
    title: 'Animations', 
    icon: <WbIncandescentIcon />,
    content: <AnimationButtonList />
  }, 
  'effects': {
    title: 'Effects', 
    icon: <PaletteIcon />, 
    content: <EffectPicker />
  }, 
  'developer': {
    title: 'Developer', 
    icon: <CodeIcon />, 
    content: <DevloperPage />
  }
}

const drawerWidth = 240;

const styles = theme => ({
  root: {
    flexGrow: 1
  },
  menuButton: {
    marginRight: theme.spacing(2),
    [theme.breakpoints.up('sm')]: {
      display: 'none',
    },
  },
  // necessary for content to be below app bar
  toolbar: theme.mixins.toolbar,
  drawerPaper: {
    width: drawerWidth,
    backgroundColor: '#464d5b'
  },
  content: {
    flexGrow: 1,
    padding: theme.spacing(3),
    [theme.breakpoints.up('sm')]: {
      width: `calc(97% - ${drawerWidth}px)`,
      marginLeft: drawerWidth,
    },
  },
  title: {
    flexGrow: 1,
  },
  appBar: {
    [theme.breakpoints.up('sm')]: {
      width: `calc(100% - ${drawerWidth}px)`,
      marginLeft: drawerWidth,
    },
  },
  drawer: {
    [theme.breakpoints.up('sm')]: {
      width: drawerWidth,
      flexShrink: 0,
    },
  },
  whiteIcon: {
    color: '#ffffff'
  }, 
  logo: {
    marginTop: theme.spacing(1),
    height: '25px', 
    width: '25px'
  }, 
  drawerTitle: {
    marginTop: theme.spacing(1.5), 
    marginLeft: '-15px'
  }, 
});

class MenuContent extends Component {
  constructor() {
    super();

    this.state = {
      power: 0, 
      mobileOpen: false, 
      contentId: 'animations'
    };
  }

  // Toggle the side menu
  handleDrawerToggle() {
    this.setState(prevState => ({
      mobileOpen: !prevState.mobileOpen
    }));
  };

  // Switch the content to a different "page" and close the menu
  handleMenuButtonClick(buttonId) {
    this.setState({
      contentId: buttonId, 
      mobileOpen: false
    })
  }

  // Fetch current power status
  componentDidMount() {
    fetch('/api/status')
      .then((response) => response.json())
      .then((data) => 
        this.setState({
          power: data.status.powerOn
        })
      );
  }

  // Send GET request to API to toggle power to the LED strip
  togglePower() {
    fetch('/api/power/toggle')
      .then((response) => response.json())
      .then((data) => 
        this.setState({
          power: data.status.powerOn
        })
      );
  }

  // Determine the current power button color based on state
  buttonColor() {
    let color;

    switch(this.state.power) {
      case 0:
        color = "red";
        break;
      case 1:
        color = "green"
        break;
      default:
        color = "red";
        break;
    }

    return color;
  }

  // Determine the current content based on state
  visibleContent() {
    return contentMap[this.state.contentId].content;
  }

  // Determine the current title based on state
  visibleTitle() {
    return contentMap[this.state.contentId].title;
  }

  render() {
    const { classes } = this.props;

    const drawer = (
      <div className='drawer'>
        <div className={classes.toolbar}>
        <List dense>
          <ListItem>
            <ListItemIcon> 
            <img src='favicon.gif' className={classes.logo}></img>
            </ListItemIcon>
            <ListItemText 
              className={classes.drawerTitle}  
              primary={
                <WhiteTextTypography variant="title">
                   LED Strip Controls
                </WhiteTextTypography>
              }/>
          </ListItem>
            </List>
        </div>
        <Divider />
        <List>
          { 
          // Construct menu from content map
          Object.keys(contentMap).map((id, i) =>
            <div id={`${id}MenuItem`}>
              <ListItem 
                button 
                selected={this.state.contentId === id}
                onClick={() => this.handleMenuButtonClick(id)}
              >
                <ListItemIcon> 
                  <div className={classes.whiteIcon}>
                    {contentMap[id].icon}
                  </div>
                </ListItemIcon>
                <ListItemText primary={
                  <WhiteTextTypography variant="body">
                    {contentMap[id].title}
                  </WhiteTextTypography>
                } />
              </ListItem>
            </div>
          )}
        </List>
      </div>
    );

    return (
      <div className={classes.root}>
        <AppBar position="fixed" className={classes.appBar}>
          <Toolbar>
          <IconButton
            color="inherit"
            aria-label="open drawer"
            edge="start"
            onClick={() => { this.handleDrawerToggle() }}
            className={classes.menuButton}
          >
            <MenuIcon />
          </IconButton>
            <Typography 
              edge="start" 
              variant="subtitle1" 
              color="inherit" 
              className={classes.title}
            >
              {this.visibleTitle()}
            </Typography>
            <IconButton 
              size="small"
              style={{
                backgroundColor: this.buttonColor()
              }} 
              onClick={() => { this.togglePower() }}
            >
              <PowerSettingsNewIcon className={classes.whiteIcon}/>
            </IconButton>
          </Toolbar>
        </AppBar>

        <nav className={classes.drawer} aria-label="menu">
        <Hidden smUp implementation="css">
          <Drawer
            container={container}
            variant="temporary"
            anchor='left'
            open={this.state.mobileOpen}
            onClose={() => { this.handleDrawerToggle() }}
            classes={{
              paper: classes.drawerPaper,
            }}
            ModalProps={{
              keepMounted: true, // Better open performance on mobile.
            }}
          >
            {drawer}
          </Drawer>
        </Hidden>
        <Hidden xsDown implementation="css">
          <Drawer
            classes={{
              paper: classes.drawerPaper,
            }}
            variant="permanent"
            open
          >
            {drawer}
          </Drawer>
        </Hidden>
      </nav>
      <main className={classes.content}>
        <div className={classes.toolbar} />
        {this.visibleContent()}
      </main>
      </div>
    );
  }
}


export default withStyles(styles)(MenuContent);
