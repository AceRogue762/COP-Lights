/**
 * A typography container which sets font color to white
 */

import Typography from '@material-ui/core/Typography';
import { withStyles } from "@material-ui/core/styles";

const WhiteTextTypography = withStyles({
    root: {
      color: "#FFFFFF"
    }
})(Typography);


export default WhiteTextTypography;