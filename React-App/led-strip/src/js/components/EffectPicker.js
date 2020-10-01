/**
 * Component to allow for setting custom colors and effects
 * 
 * - Selected color and effect are pushed to /api/effects/set.
 */

import React, { Component } from "react";
import { HuePicker, AlphaPicker } from 'react-color';
import PlayArrowIcon from '@material-ui/icons/PlayArrow';
import IconButton from '@material-ui/core/IconButton';
import { withStyles } from "@material-ui/core/styles";

import WhiteTextTypography from './WhiteTextTypography';

const styles = theme => ({
  playButton: {
    marginLeft: '-25px'
  }, 
  playIcon: {
    color: "green"
  }
});

class EffectPicker extends Component {
    constructor() {
      super();
  
      this.state = {
        primaryColor: {
          r: 0, 
          g: 0, 
          b: 0, 
          a: 1
        }
      };
    }

    // Set primary color state
    handlePrimaryColorChange = (color) => {
      let r = color.rgb.r;
      let g = color.rgb.g;
      let b = color.rgb.b;
      let a = this.state.primaryColor.a;

      this.setState({ 
        primaryColor: {
          r: r, 
          g: g, 
          b: b, 
          a: a
        }
      });
    };

    // Set primary color alpha value
    handlePrimaryAlphaChange = (color) => {
      let r = this.state.primaryColor.r;
      let g = this.state.primaryColor.g;
      let b = this.state.primaryColor.b;
      let a = color.rgb.a;

      this.setState({ 
        primaryColor: {
          r: r, 
          g: g, 
          b: b, 
          a: a
        }
      });
    };

    // Send effect to device
    playEffect() {
      let r = this.state.primaryColor.r;
      let g = this.state.primaryColor.g;
      let b = this.state.primaryColor.b;
      let a = this.state.primaryColor.a;

      // Scale RGB values by alpha
      let scaledR = r * a;
      let scaledG = g * a;
      let scaledB = b * a;

      // Construct API URL
      let url = `/api/effects/set?type=solidColor&r=${scaledR}&g=${scaledG}&b=${scaledB}`

      // Send API request
      fetch(url);
    };

    render() {
      const { classes } = this.props;

      return (
        <div>
          <WhiteTextTypography 
            variant="h5" 
            color="textSecondary" 
            gutterBottom
          >
            Primary Color
          </WhiteTextTypography>
          <HuePicker 
            color={ this.state.primaryColor } 
            onChange={ this.handlePrimaryColorChange }
          />
          <AlphaPicker 
            color={ this.state.primaryColor }
            onChange={ this.handlePrimaryAlphaChange }
          />
          <IconButton 
            className={classes.playButton}
            onClick={() => { this.playEffect() }}
          >
            <PlayArrowIcon 
              className={classes.playIcon}
              fontSize="large" 
            />
          </IconButton>
        </div>
      );
    }
  }
  
  export default withStyles(styles)(EffectPicker);
  